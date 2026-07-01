// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pluginmanager.h"

#include "dccfactory.h"
#include "dccmanager.h"
#include "dccobject_p.h"
#include "dccpluginloader.h"

#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFileInfo>
#include <QLoggingCategory>
#include <QPluginLoader>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlFile>
#include <QRunnable>
#include <QSet>
#include <QSettings>
#include <QTimer>
#include <QtConcurrent>
#include <QtConcurrentRun>

#include <QDBusConnection>

namespace dccV25 {

const static QString TranslateReadDir = QStringLiteral(TRANSLATE_READ_DIR);

// Run async data loading in thread pool
class LoadDataTask : public QRunnable
{
public:
    explicit LoadDataTask(DccPluginLoader *loader, DccPluginManager *manager)
        : QRunnable()
        , m_loader(loader)
        , m_manager(manager)
    {
    }

    void run() override
    {
        if (m_manager->isDeleting()) {
            return;
        }
        QElapsedTimer timer;
        timer.start();
        m_loader->loadData();
        m_loader->setLog("load data finished. elapsed time :" + QString::number(timer.elapsed()));
        m_loader->transitionStatus(DccPluginLoader::DataLoad);
        if (m_manager->isDeleting()) {
            return;
        }
        m_loader->createData();
        if (m_manager->isDeleting()) {
            return;
        }
        m_loader->moveThread();
        m_loader->setLog("create data finished. elapsed time :" + QString::number(timer.elapsed()));
        m_loader->transitionStatus(DccPluginLoader::DataEnd);
    }

private:
    DccPluginLoader *m_loader;
    DccPluginManager *m_manager;
};

// DataRequestTask - DBus get/set 请求任务
class DataRequestTask : public QRunnable
{
public:
    DataRequestTask(DccPluginLoader *loader, const QVariantMap &param,
                    const QDBusMessage &message, bool isGet,
                    DccPluginManager *manager)
        : m_loader(loader), m_param(param), m_message(message)
        , m_isGet(isGet), m_manager(manager)
    {
        setAutoDelete(true);
    }

    void run() override
    {
        // 如果还未到达 DataEnd，需要等待加载
        if (!(m_loader->status() & DccPluginLoader::DataEnd)) {
            // 设置标志
            m_loader->setPendingDataRequest(true);

            // 等待 DataEnd
            waitForDataEnd();
        }
        qWarning()<<__LINE__<<__FUNCTION__<<m_loader->name()<<m_isGet<<m_param;
        // 回复 DBus
        if (m_loader->status() & DccPluginLoader::DataEnd) {
            QVariantMap result;
            if (auto *factory = m_loader->factory()) {
                if (m_isGet) {
                    result = factory->get(m_param);
                } else {
                    result = factory->set(m_param);
                }
            }
            QDBusConnection::sessionBus().send(m_message.createReply(result));
        } else {
            // 超时或加载失败
            QDBusConnection::sessionBus().send(
                m_message.createErrorReply("org.deepin.dde.ControlCenter1.Error.Timeout",
                                          "Plugin load timeout: " + m_loader->name()));
        }
    }

private:
    void waitForDataEnd()
    {
        QEventLoop loop;
        QTimer timeout;
        timeout.setSingleShot(true);
        timeout.setInterval(10000);  // 10秒超时

        QObject context;

        auto conn = QObject::connect(m_loader, &DccPluginLoader::statusChanged,
                                    &context, [&](DccPluginLoader *, uint status) {
            if (status & DccPluginLoader::DataEnd) {
                loop.quit();
            } else if (status & DccPluginLoader::PluginErrMask) {
                loop.quit();
            }
        }, Qt::QueuedConnection);

        QObject::connect(&timeout, &QTimer::timeout, &context, [&]{ loop.quit(); });
        timeout.start();

        // 发射信号触发状态机继续（Qt::QueuedConnection 自动在主线程执行 loadPlugin）
        Q_EMIT m_manager->requestAdvancePlugin(m_loader);

        loop.exec();
        QObject::disconnect(conn);
    }

    DccPluginLoader *m_loader;
    QVariantMap m_param;
    QDBusMessage m_message;
    bool m_isGet;
    DccPluginManager *m_manager;
};

DccPluginManager::DccPluginManager(DccManager *parent)
    : QObject(parent)
    , m_manager(parent)
    , m_rootModule(nullptr)
    , m_threadPool(nullptr)
    , m_isDeleting(false)
{
    connect(m_manager, &DccManager::hideModuleChanged, this, &DccPluginManager::onHideModuleChanged);

    // 连接 requestAdvancePlugin 信号到 loadPlugin（只需绑定一次）
    connect(this, &DccPluginManager::requestAdvancePlugin,
            this, &DccPluginManager::loadPlugin, Qt::QueuedConnection);
}

DccPluginManager::~DccPluginManager()
{
    cancelLoad();
    qDeleteAll(m_plugins);
    m_plugins.clear();
}

QThreadPool *DccPluginManager::threadPool()
{
    if (!m_threadPool) {
        m_threadPool = new QThreadPool(); // No parent to avoid double-delete
    }
    return m_threadPool;
}

// 根据状态执行插件流程
void DccPluginManager::loadPlugin(DccPluginLoader *loader)
{
    if (isDeleting()) {
        return;
    }
    if (loader->status() & DccPluginLoader::PluginEnd) {
        if (loadFinished()) {
            Q_EMIT loadAllFinished();
            cancelLoad();
        }
    } else if (loader->status() & DccPluginLoader::MainObjEnd) {
        loader->addMainObject();
        if (loader->mainObj()) {
            Q_EMIT addObject(loader->mainObj());
        }
        if (loader->soObj()) {
            Q_EMIT addObject(loader->soObj());
        }
        loader->transitionStatus(DccPluginLoader::PluginEnd);
    } else if ((loader->status() & (DccPluginLoader::ModuleEnd | DccPluginLoader::DataEnd | DccPluginLoader::MainObjLoad)) == (DccPluginLoader::ModuleEnd | DccPluginLoader::DataEnd)) {
        loader->transitionStatus(DccPluginLoader::MainObjLoad);
        loader->createDccObject();
        loader->updateParent();
        loader->loadMain();
        loader->transitionStatus(DccPluginLoader::MainObjEnd);
    } else if (((loader->status() & (DccPluginLoader::ModuleEnd | DccPluginLoader::DataBegin)) == DccPluginLoader::ModuleEnd)
               || ((m_loadMode == OnlyData && loader->hasPendingDataRequest())
                   && ((loader->status() & (DccPluginLoader::MetaDataEnd | DccPluginLoader::DataBegin)) == DccPluginLoader::MetaDataEnd))) {
        loader->transitionStatus(DccPluginLoader::DataBegin);
        threadPool()->start(new LoadDataTask(loader, this));
    } else if (((loader->status() & (DccPluginLoader::MetaDataEnd | DccPluginLoader::ModuleLoad)) == DccPluginLoader::MetaDataEnd) && m_loadMode != OnlyData) {
        // MetaDataEnd 状态
        // if (m_loadMode == OnlyData && !loader->hasPendingDataRequest()) {
        //     // OnlyData 模式且非 DBus 目标：停在 MetaDataEnd，等待 requestAdvancePlugin 信号
        //     return;
        // }
        // 否则继续加载（OnlyData+DBus目标 或 Full模式）
        loader->transitionStatus(DccPluginLoader::ModuleLoad);
        if (loader->loadModule()) {
            Q_EMIT addObject(loader->module());
            loader->transitionStatus(DccPluginLoader::ModuleEnd);
        } else {
            loader->transitionStatus(DccPluginLoader::ModuleEnd | DccPluginLoader::PluginEnd);
        }
    } else if (!(loader->status() & DccPluginLoader::MetaDataEnd)) {
        if (loader->loadMetaData()) {
            loader->transitionStatus(DccPluginLoader::MetaDataEnd);
        } else {
            loader->transitionStatus(DccPluginLoader::MetaDataEnd | DccPluginLoader::PluginEnd);
        }
    }
}

void DccPluginManager::loadModules(DccObject *root, bool async, const QStringList &dirs, QQmlEngine *engine)
{
    Q_UNUSED(async)
    if (!root)
        return;
    m_rootModule = root;
    m_engine = engine;
    qCDebug(dccLog()) << "plugin dir:" << dirs;

    QFileInfoList pluginList;
    for (const auto &dir : dirs) {
        QDir plugindir(dir);
        if (plugindir.exists()) {
            if (!plugindir.isEmpty(QDir::Files)) {
                pluginList += QFileInfo(plugindir.absolutePath());
            }
            pluginList += plugindir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        }
    }

    const QStringList groupPlugins({ "system", "device" }); // 优先加载只是组的插件
    QList<DccPluginLoader *> loaders;

    for (auto &lib : pluginList) {
        const QString &filepath = lib.absoluteFilePath();
        auto filename = lib.fileName();
        DccPluginLoader *loader = new DccPluginLoader(lib.baseName(), filepath, this);

        // Set version type based on path
        DccPluginLoader::TypeFlags type = DccPluginLoader::T_Unknown;
        if (filepath.contains("_v1.1")) {
            type = DccPluginLoader::T_V1_1;
        } else if (filepath.contains("_v1.0")) {
            type = DccPluginLoader::T_V1_0;
        }
        loader->setType(type);

        // Connect signals
        connect(loader, &DccPluginLoader::statusChanged, this, &DccPluginManager::onPluginStatusChanged, Qt::QueuedConnection);

        if (groupPlugins.contains(filename)) {
            loaders.prepend(loader);
        } else {
            loaders.append(loader);
        }
    }

    m_plugins = loaders;

    // Start loading all plugins
    for (auto &&loader : m_plugins) {
        loadPlugin(loader);
    }
}

void DccPluginManager::cancelLoad()
{
    if (m_threadPool) {
        qCWarning(dccLog()) << "delete threadPool";
        m_threadPool->clear();
        if (!m_threadPool->waitForDone(3000)) {
            qCWarning(dccLog()) << "thread pool waitForDone timeout, some tasks may still be running";
        }
        delete m_threadPool;
        qCWarning(dccLog()) << "delete threadPool finish";
        m_threadPool = nullptr;
    }
}

bool DccPluginManager::loadFinished() const
{
    for (auto &&loader : m_plugins) {
        if (!loader->isFinished()) {
            return false;
        }
    }
    return !m_plugins.isEmpty();
}

void DccPluginManager::beginDelete()
{
    m_isDeleting.store(true);
}

QQmlEngine *DccPluginManager::engine()
{
    return m_engine;
}

DccObject *DccPluginManager::rootModule()
{
    return m_rootModule;
}

bool DccPluginManager::hidden(const QString &name)
{
    return m_manager->hideModule().contains(name);
}

void DccPluginManager::get(const QString &module, const QVariantMap &param, const QDBusMessage &message)
{
    DccPluginLoader *loader = findLoader(module);
    if (!loader) {
        QDBusConnection::sessionBus().send(
            message.createErrorReply("org.deepin.dde.ControlCenter1.Error.NotFound",
                                    "Module not found: " + module));
        return;
    }

    // 统一由 DataRequestTask 处理（等待 + 执行）
    threadPool()->start(new DataRequestTask(loader, param, message, true, this));
}

void DccPluginManager::set(const QString &module, const QVariantMap &param, const QDBusMessage &message)
{
    DccPluginLoader *loader = findLoader(module);
    if (!loader) {
        QDBusConnection::sessionBus().send(
            message.createErrorReply("org.deepin.dde.ControlCenter1.Error.NotFound",
                                    "Module not found: " + module));
        return;
    }

    threadPool()->start(new DataRequestTask(loader, param, message, false, this));
}

DccPluginLoader *DccPluginManager::findLoader(const QString &module) const
{
    for (auto *loader : m_plugins) {
        if (loader->name() == module) {
            return loader;
        }
    }
    return nullptr;
}

void DccPluginManager::switchToFullMode()
{
    if (m_loadMode == Full) return;
    m_loadMode = Full;
    qCInfo(dccLog) << "Switching to full load mode";

    // 从各插件当前状态继续推进
    for (auto &&loader : m_plugins) {
        if (loader->isFinished()) continue;
        // 重置标志，避免影响全量加载
        loader->setPendingDataRequest(false);
        loadPlugin(loader);
    }
}

void DccPluginManager::onPluginStatusChanged(DccPluginLoader *loader, uint status)
{
    if (isDeleting()) {
        return;
    }
    if ((status & DccPluginLoader::PluginEndMask)) {
        loadPlugin(loader);
    }
}

void DccPluginManager::onHideModuleChanged(const QSet<QString> &hideModule)
{
    for (auto &&loader : m_plugins) {
        if ((loader->status() & DccPluginLoader::PluginEnd) && !loader->module() && !hideModule.contains(loader->name())) {
            loader->updateVisible(true);
        }
    }
}

} // namespace dccV25
