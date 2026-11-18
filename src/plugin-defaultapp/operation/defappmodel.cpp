// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "defappmodel.h"

#include "QtQml/qqml.h"
#include "categorymodel.h"
#include "dccfactory.h"
#include "defappworker.h"
#include "defappworkerold.h"
#include "mimedbusproxyold.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(DdcDefaultAppModel, "dcc-defaultapp-model")

// 数据通道的键与类别下标一一对应。键名是对外协议，改动即破坏兼容。
namespace {

struct KeyInfo {
    const char *key;
    int category;
    const char *backendName;
};

const QVector<KeyInfo> &keyTable()
{
    static const QVector<KeyInfo> table = {
        { "browser", DefAppModel::Browser, "Browser" },
        { "mail", DefAppModel::Mail, "Mail" },
        { "text", DefAppModel::Text, "Text" },
        { "music", DefAppModel::Music, "Music" },
        { "video", DefAppModel::Video, "Video" },
        { "picture", DefAppModel::Picture, "Picture" },
        { "terminal", DefAppModel::Terminal, "Terminal" },
    };
    return table;
}

const KeyInfo *keyInfo(const QString &key)
{
    for (const auto &info : keyTable()) {
        if (key == QLatin1String(info.key)) {
            return &info;
        }
    }
    return nullptr;
}

} // namespace

DefAppModel::DefAppModel(QObject *parent)
    : QObject(parent)
    , m_work(nullptr)
    , m_oldwork(nullptr)
    , m_isOldInterface(false)
{
    // 构造函数只建对象树与信号连接，不启动依赖事件循环的后端工作。
    // 生命周期钩子（active/deactive）由框架在主线程调用。
    for (int i = 0; i < Count; i++) {
        m_categoryModel[i] = new CategoryModel(new Category(this));
    }

    m_isOldInterface = MimeDBusProxyOld::isRegisted();
    if (m_isOldInterface) {
        m_oldwork = new DefAppWorkerOld(this, this);
        for (int i = 0; i < Count; i++) {
            connect(m_categoryModel[i], &CategoryModel::requestCreateFile,
                    m_oldwork, &DefAppWorkerOld::onCreateFile);
            connect(m_categoryModel[i], &CategoryModel::requestDelUserApp,
                    m_oldwork, &DefAppWorkerOld::onDelUserApp);
            connect(m_categoryModel[i], &CategoryModel::requestSetDefaultApp,
                    m_oldwork, &DefAppWorkerOld::onSetDefaultApp);
        }
    } else {
        m_work = new DefAppWorker(this, this);
        for (int i = 0; i < Count; i++) {
            connect(m_categoryModel[i], &CategoryModel::requestCreateFile,
                    m_work, &DefAppWorker::onCreateFile);
            connect(m_categoryModel[i], &CategoryModel::requestDelUserApp,
                    m_work, &DefAppWorker::onDelUserApp);
            connect(m_categoryModel[i], &CategoryModel::requestSetDefaultApp,
                    m_work, &DefAppWorker::onSetDefaultApp);
        }
    }
}

DefAppModel::~DefAppModel()
{
    for (int i = 0; i < Count; i++) {
        m_categoryModel[i]->deleteLater();
    }
}

void DefAppModel::initialized()
{
    // 主线程、页面加载之前：注册 QML 类型只做一次
    static bool registered = false;
    if (registered) {
        return;
    }
    registered = true;
    qmlRegisterType<CategoryModel>("org.deepin.dcc.defApp", 1, 0, "CategoryModel");
}

void DefAppModel::startBackend()
{
    if (m_backendStarted) {
        return;
    }
    m_backendStarted = true;

    // 后端变更后把当前默认项同步到通道，供订阅方感知。
    // 只有显示默认项变化会触发：精简实现不做后端值/显示值分离。
    for (int i = 0; i < Count; ++i) {
        Category *category = categoryAt(i);
        if (!category) {
            continue;
        }
        // 只在首次 startBackend 时连接一次（m_backendStarted 守卫），
        // 这里不能用 Qt::UniqueConnection：lambda 作槽不支持该标志。
        connect(category, &Category::defaultChanged, this, [this, i] {
            Category *cat = categoryAt(i);
            if (!cat) {
                return;
            }
            const KeyInfo &info = keyTable().at(i);
            Q_EMIT propertiesChanged({ { QLatin1String(info.key), cat->getDefault().Id } });
        });
    }

    if (m_oldwork) {
        m_oldwork->active();
        m_oldwork->onGetListApps();
    } else if (m_work) {
        m_work->active();
        m_work->onGetListApps();
    }
}

void DefAppModel::active()
{
    startBackend();
}

void DefAppModel::deactive()
{
    if (m_oldwork) {
        m_oldwork->deactive();
    }
    if (m_work) {
        m_work->deactive();
    }
}

dccV25::DataReply DefAppModel::get(const QVariantMap &param, const QString &caller)
{
    Q_UNUSED(caller)

    // 保留键 "keys"：返回本模块支持的键表，独占语义——出现即忽略其他键。
    // 不要求后端已就绪：键表是静态协议，无需等读取完成。
    if (param.contains(QStringLiteral("keys"))) {
        QStringList keys;
        keys.reserve(keyTable().size());
        for (const auto &info : keyTable()) {
            keys.append(QLatin1String(info.key));
        }
        return { dccV25::DataStatus::Ok, { { QStringLiteral("keys"), keys } }, {} };
    }

    if (param.isEmpty()) {
        return { dccV25::DataStatus::InvalidArgs, {}, QStringLiteral("empty request") };
    }
    // 请求值忽略，只取键集合；未知键整体拒绝
    QStringList requested;
    for (auto it = param.cbegin(); it != param.cend(); ++it) {
        if (!keyInfo(it.key())) {
            return { dccV25::DataStatus::InvalidArgs, {},
                     QStringLiteral("unknown key: ") + it.key() };
        }
        requested.append(it.key());
    }
    // 后端还没开始读：没有可信值可报，不能拿空串冒充"未设置"
    if (!m_backendStarted) {
        return { dccV25::DataStatus::NotReady, {}, QStringLiteral("backend not started") };
    }

    QVariantMap result;
    for (const QString &key : requested) {
        const KeyInfo *info = keyInfo(key);
        Category *category = categoryAt(info->category);
        if (!category) {
            return { dccV25::DataStatus::NotSupported, {},
                     QStringLiteral("category unavailable: ") + key };
        }
        // 精简实现：以类别当前默认项为准；未设置时为空串
        result.insert(key, category->getDefault().Id);
    }
    return { dccV25::DataStatus::Ok, result, {} };
}

dccV25::DataReply DefAppModel::set(const QVariantMap &param, const QString &caller)
{
    Q_UNUSED(caller)

    if (param.isEmpty()) {
        return { dccV25::DataStatus::InvalidArgs, {}, QStringLiteral("empty request") };
    }
    // 先整体校验：未知键或空值直接拒绝，避免部分写入
    for (auto it = param.cbegin(); it != param.cend(); ++it) {
        if (!keyInfo(it.key())) {
            return { dccV25::DataStatus::InvalidArgs, {},
                     QStringLiteral("unknown key: ") + it.key() };
        }
        if (it.value().toString().isEmpty()) {
            return { dccV25::DataStatus::InvalidArgs, {},
                     QStringLiteral("empty value for key: ") + it.key() };
        }
    }

    // 逐项执行：空值表示已提交到写入路径，非空为该项错误
    QVariantMap result;
    for (auto it = param.cbegin(); it != param.cend(); ++it) {
        const KeyInfo *info = keyInfo(it.key());
        if (applyDefaultById(info->category, it.value().toString())) {
            result.insert(it.key(), QString());
        } else {
            result.insert(it.key(), QStringLiteral("unknown application: ")
                                            + it.value().toString());
        }
    }
    return { dccV25::DataStatus::Ok, result, {} };
}

bool DefAppModel::applyDefaultById(int categoryIndex, const QString &desktopId)
{
    Category *category = categoryAt(categoryIndex);
    if (!category || desktopId.isEmpty()) {
        return false;
    }

    // 目标应用必须在该类别的候选列表里，否则不写后端
    const auto apps = category->getappItem();
    auto it = std::find_if(apps.cbegin(), apps.cend(), [&desktopId](const App &app) {
        return app.Id == desktopId;
    });
    if (it == apps.cend()) {
        return false;
    }

    // 类别名必须取自键表：Category::getName() 由 worker 首次读取时才设置，
    // 若此时还没读完，getName() 为空会让合法的 Set 被拒。
    const QString categoryName = QLatin1String(keyTable().at(categoryIndex).backendName);

    // 走 UI 相同的写入路径；成功与否由 Worker 的完成回调更新 Category
    if (m_oldwork) {
        m_oldwork->onSetDefaultApp(categoryName, *it);
    } else if (m_work) {
        m_work->onSetDefaultApp(categoryName, *it);
    } else {
        return false;
    }
    return true;
}

DCC_FULL_FACTORY_CLASS(DefAppModel)

#include "defappmodel.moc"
