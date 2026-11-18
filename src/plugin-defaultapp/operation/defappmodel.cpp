// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "defappmodel.h"

#include "QtQml/qqml.h"
#include "categorymodel.h"
#include "dccfactory.h"
#include "defaultappdata.h"
#include "defappworker.h"
#include "defappworkerold.h"
#include "mimedbusproxyold.h"
#include <QHash>
#include <QLoggingCategory>
#include <QThread>
#include <QTimer>


Q_LOGGING_CATEGORY(DdcDefaultAppModel, "dcc-defaultapp-model")

DefAppModel::DefAppModel(QObject *parent)
    : QObject(parent)
    , m_work(nullptr)
    , m_oldwork(nullptr)
    , m_dataChannel(new DefaultAppData(this))
    , m_retryTimer(new QTimer(this))
    , m_isOldInterface(false)
{
    // 构造函数只建对象树，不启动依赖主线程事件循环的后端工作
    for (int i = 0; i < Count; i++) {
        m_categoryModel[i] = new CategoryModel(new Category(this));
    }

    m_isOldInterface = MimeDBusProxyOld::isRegisted();
    if (m_isOldInterface) {
        m_oldwork = new DefAppWorkerOld(this, this);
    } else {
        m_work = new DefAppWorker(this, this);
    }

    for (int i = 0; i < Count; i++) {
        if (m_oldwork) {
            connect(m_categoryModel[i], &CategoryModel::requestCreateFile, m_oldwork, &DefAppWorkerOld::onCreateFile);
            connect(m_categoryModel[i], &CategoryModel::requestDelUserApp, m_oldwork, &DefAppWorkerOld::onDelUserApp);
            connect(m_categoryModel[i], &CategoryModel::requestSetDefaultApp, m_oldwork, &DefAppWorkerOld::onSetDefaultApp);
        } else if (m_work) {
            connect(m_categoryModel[i], &CategoryModel::requestCreateFile, m_work, &DefAppWorker::onCreateFile);
            connect(m_categoryModel[i], &CategoryModel::requestDelUserApp, m_work, &DefAppWorker::onDelUserApp);
            connect(m_categoryModel[i], &CategoryModel::requestSetDefaultApp, m_work, &DefAppWorker::onSetDefaultApp);
        }
    }

    // 读取失败的类别按退避间隔重试，避免一次瞬时错误让模块永久 NotReady；
    // 退避封顶后变成低频探测，不会无限施加压力。
    m_retryTimer->setSingleShot(true);
    connect(m_retryTimer, &QTimer::timeout, this, &DefAppModel::retryUnresolvedCategories);

    // 数据通道从模型读取快照后转发变更通知
    connect(m_dataChannel, &DefaultAppData::propertiesChanged,
            this, &DefAppModel::propertiesChanged);
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
    if (m_backendStarted || m_deactivated) {
        return;
    }
    m_backendStarted = true;

    QHash<int, Category *> categories;
    for (int i = 0; i < Count; ++i) {
        Category *cat = categoryAt(i);
        if (!cat) {
            continue;
        }
        categories.insert(i, cat);
        // 后端真实值、显示默认项、读取状态、候选集合任一变化都要重算快照
        connect(cat, &Category::backendDefaultChanged, this, &DefAppModel::refreshSnapshot,
                Qt::UniqueConnection);
        connect(cat, &Category::defaultChanged, this, &DefAppModel::refreshSnapshot,
                Qt::UniqueConnection);
        connect(cat, &Category::readStateChanged, this, &DefAppModel::refreshSnapshot,
                Qt::UniqueConnection);
        connect(cat, &Category::addedUserItem, this, &DefAppModel::refreshSnapshot,
                Qt::UniqueConnection);
        connect(cat, &Category::removedUserItem, this, &DefAppModel::refreshSnapshot,
                Qt::UniqueConnection);
    }
    // 先登记类别，后续任何通知都能映射到具体键
    m_dataChannel->setCategories(categories);

    if (m_oldwork) {
        m_oldwork->active();
        m_oldwork->onGetListApps();
    } else if (m_work) {
        m_work->active();
        m_work->onGetListApps();
    } else {
        m_dataChannel->markUnavailable(QStringLiteral("no default-app backend"));
    }
}

void DefAppModel::refreshSnapshot()
{
    if (m_deactivated) {
        // 已停用：晚到的后端通知不得把快照重新置为可用
        return;
    }
    m_dataChannel->refresh();

    // 后端恢复后无需依赖额外的 Change 通知：读取状态与值变化都会走到这里。
    // 只有全部类别读取成功才清空退避；"读取中"（Unread）既不算成功也不是失败，
    // 否则重试把 Failed 置为 Unread 时就会把退避重置，导致退避永不增长。
    bool anyFailed = false;
    bool anyUnread = false;
    for (int i = 0; i < Count; ++i) {
        Category *cat = categoryAt(i);
        if (!cat) {
            continue;
        }
        switch (cat->readState()) {
        case Category::ReadState::Ok:
            break;
        case Category::ReadState::Failed:
            anyFailed = true;
            break;
        case Category::ReadState::Unread:
            anyUnread = true;
            break;
        }
    }

    if (!anyFailed && !anyUnread) {
        // 全部读取成功：退避回到初始值，停掉定时器
        m_retryDelayMs = kInitialRetryDelayMs;
        m_retryTimer->stop();
        return;
    }
    if (anyFailed && !m_retryTimer->isActive()) {
        // 只有失败才安排重试；读取中的类别等后端回包，不叠加请求
        m_retryTimer->start(m_retryDelayMs);
        m_retryDelayMs = nextRetryDelay(m_retryDelayMs);
    }
}

void DefAppModel::retryUnresolvedCategories()
{
    if (m_deactivated) {
        return;
    }
    if (!m_backendStarted) {
        startBackend();
        return;
    }

    bool anyRetrying = false;
    for (int i = 0; i < Count; ++i) {
        Category *cat = categoryAt(i);
        if (!cat || cat->readState() != Category::ReadState::Failed) {
            continue;
        }
        // Unread 表示"读取中"：Get 在此期间返回 NotReady，而不是沿用旧的失败结论。
        // 置为 Unread 会同步触发 refreshSnapshot()，读取中不会再排下一次重试，
        // 因此后端处理期间不会叠加请求。
        cat->setReadState(Category::ReadState::Unread);
        anyRetrying = true;
    }
    if (!anyRetrying) {
        return;
    }

    // 重试复用既有读取入口：worker 会重新拉取全部类别，
    // 已成功的类别重复写入不会产生额外通知（setter 内部去重）。
    if (m_oldwork) {
        m_oldwork->onGetListApps();
    } else if (m_work) {
        m_work->onGetListApps();
    }
}

void DefAppModel::active()
{
    // 重新激活时允许再次启动与重试，并重新获得完整的退避预算
    m_deactivated = false;
    m_retryDelayMs = kInitialRetryDelayMs;
    startBackend();
    refreshSnapshot();
}

void DefAppModel::deactive()
{
    // 置位后 refreshSnapshot() 不再把快照改成可用，否则晚到的后端通知会重新激活通道
    m_deactivated = true;
    m_retryTimer->stop();
    if (m_oldwork) {
        m_oldwork->deactive();
    }
    if (m_work) {
        m_work->deactive();
    }
    m_dataChannel->markUnavailable(QStringLiteral("backend deactivated"));
}

dccV25::DataReply DefAppModel::get(const QVariantMap &param, const QString &caller,
                                   const QString &callerPath)
{
    Q_UNUSED(caller)
    Q_UNUSED(callerPath)
    // 工作线程调用：首次读取与失败重试都必须回到主线程发起
    QMetaObject::invokeMethod(this, [this] {
        if (!m_backendStarted) {
            startBackend();
            return;
        }
        // 只对仍失败的类别重试，已成功的类别不必重复打后端
        retryUnresolvedCategories();
    }, Qt::QueuedConnection);
    return m_dataChannel->read(param);
}

dccV25::DataReply DefAppModel::set(const QVariantMap &param, const QString &caller,
                                   const QString &callerPath)
{
    Q_UNUSED(caller)
    Q_UNUSED(callerPath)

    // 先整体校验：未知键或空值直接拒绝，避免部分写入
    const auto validated = DefaultAppData::validateSet(param);
    if (validated.status != dccV25::DataStatus::Ok) {
        return validated;
    }
    // 逐键校验由 lookupApp 内部按 resolved 判定：
    // 只要目标键已成功读取，即使其他键仍在读取也可写入，不必等整模块就绪。

    // 逐项执行，返回每个请求键的结果：空值表示已提交，非空为错误信息
    QVariantMap result;
    for (auto it = param.cbegin(); it != param.cend(); ++it) {
        const QString desktopId = it.value().toString();
        if (applyDefaultById(it.key(), desktopId)) {
            result.insert(it.key(), QString());
        } else {
            result.insert(it.key(), QStringLiteral("unknown application: ") + desktopId);
        }
    }
    return { dccV25::DataStatus::Ok, result, {} };
}

bool DefAppModel::applyDefaultById(const QString &key, const QString &desktopId)
{
    // 键到后端类别名只有一份定义（DefaultAppData 的键表），这里不再重复维护
    const QString categoryName = DefaultAppData::backendNameForKey(key);
    if (categoryName.isEmpty()) {
        return false;
    }

    // 校验目标应用：从数据通道的受锁快照取，不读主线程的 Category。
    App app;
    if (!m_dataChannel->lookupApp(key, desktopId, &app)) {
        return false;
    }

    // 写入必须回到主线程：Category/CategoryModel/Worker 都是主线程对象
    if (thread() != QThread::currentThread()) {
        QMetaObject::invokeMethod(this, [this, categoryName, app] {
            writeDefaultToBackend(categoryName, app);
        }, Qt::QueuedConnection);
        return true;
    }
    writeDefaultToBackend(categoryName, app);
    return true;
}

void DefAppModel::writeDefaultToBackend(const QString &categoryName, const App &app)
{
    // 走 Worker 既有写入路径，与 UI 行为一致；快照只在后端确认后由 defaultChanged 刷新
    if (m_oldwork) {
        m_oldwork->onSetDefaultApp(categoryName, app);
    } else if (m_work) {
        m_work->onSetDefaultApp(categoryName, app);
    }
}

DCC_FULL_FACTORY_CLASS(DefAppModel)

#include "defappmodel.moc"
