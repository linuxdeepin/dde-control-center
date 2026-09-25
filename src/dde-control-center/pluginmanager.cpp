// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pluginmanager.h"

#include "dccfactory.h"
#include "dccmanager.h"
#include "dccobject_p.h"
#include "dccdsappletmanager.h"
#include "dccpluginloader.h"

#include <QDebug>
#include <QDir>
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
#include <QtConcurrent>
#include <QtConcurrentRun>

// module同步加载尽快显示，main异步加载不阻塞主线程
// #define ASYNC_MODULE
#define ASYNC_MAIN

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
        m_loader->loadData();
        m_loader->transitionStatus(DccPluginLoader::DataLoad);
        if (m_manager->isDeleting()) {
            return;
        }
        // createData和moveThread需要完整，都执行或都不执行
        m_loader->createData();
        m_loader->moveThread();
        m_loader->transitionStatus(DccPluginLoader::DataEnd);
    }

private:
    DccPluginLoader *m_loader;
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
}

DccPluginManager::~DccPluginManager()
{
    cancelLoad();
    qDeleteAll(m_plugins);
    m_plugins.clear();
}

void DccPluginManager::setPlugins(const QStringList &plugins)
{
    m_pluginsToLoad = plugins;
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
        m_loadTimer.finishPlugin(loader->name());
        checkNavigationFinished();
        checkLoadFinished();
    } else if (loader->status() & DccPluginLoader::MainObjEnd) {
        loader->addMainObject();
        if (loader->mainObj()) {
            Q_EMIT addObject(loader->mainObj());
        }
        if (loader->soObj()) {
            Q_EMIT addObject(loader->soObj());
        }
        loader->transitionStatus(DccPluginLoader::PluginEnd);
    } else if ((loader->status() & (DccPluginLoader::DataEnd | DccPluginLoader::MainObjLoad)) == DccPluginLoader::DataEnd) {
        loader->createDccObject();
        loader->updateParent();
#ifdef ASYNC_MAIN
        m_asyncQueue.enqueue(loader);
        startNextAsync();
#else
        loader->loadMain();
#endif
    } else if ((loader->status() & (DccPluginLoader::ModuleEnd | DccPluginLoader::DataBegin)) == DccPluginLoader::ModuleEnd) {
        if (!(loader->status() & DccPluginLoader::ModuleAdd) && loader->module()) {
            if (!loader->module()->parent()) {
                loader->module()->setParent(rootModule());
            }
            Q_EMIT addObject(loader->module());
            Q_EMIT moduleLoaded(loader->name());
            loader->transitionStatus(DccPluginLoader::ModuleAdd);
        }
        if (!loader->isVisibleToApp()) {
            loader->setLog("create module finished, module is hidden");
            loader->transitionStatus(DccPluginLoader::PluginEnd);
        }
        checkNavigationFinished();
    } else if ((loader->status() & (DccPluginLoader::MetaDataEnd | DccPluginLoader::ModuleLoad)) == DccPluginLoader::MetaDataEnd) {
#ifdef ASYNC_MODULE
        m_asyncQueue.enqueue(loader);
        startNextAsync();
#else
        loader->loadModule();
#endif
    } else {
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
    m_loadTimer.start();
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

    QList<DccPluginLoader *> loaders;
    QSet<QString> matchedPlugins;

    for (auto &lib : pluginList) {
        const QString &filepath = lib.absoluteFilePath();
        const auto pluginName = lib.baseName();
        if (!m_pluginsToLoad.isEmpty() && !m_pluginsToLoad.contains(pluginName)) {
            continue;
        }
        matchedPlugins.insert(pluginName);
        DccPluginLoader *loader = new DccPluginLoader(lib.baseName(), filepath, this);
        m_loadTimer.addPlugin(loader->name());

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

        loaders.append(loader);
    }

    for (const auto &plugin : m_pluginsToLoad) {
        if (!matchedPlugins.contains(plugin)) {
            qCWarning(dccLog()) << "Requested plugin was not found:" << plugin;
        }
    }

    m_plugins = loaders;
    m_navigationFinished = false;
    m_allLoadFinished = false;

    // Start loading all plugins
    for (auto &&loader : m_plugins) {
        loadPlugin(loader);
    }
    checkNavigationFinished();
}

void DccPluginManager::checkNavigationFinished()
{
    if (m_navigationFinished) {
        return;
    }

    for (auto &&loader : m_plugins) {
        const auto status = loader->status();
        if (!(status & (DccPluginLoader::ModuleEnd | DccPluginLoader::PluginEnd))) {
            return;
        }
    }

    m_navigationFinished = true;
    DccAppTimeline::instance().log(QStringLiteral("navigation-ready"));
    Q_EMIT navigationReady();
}

void DccPluginManager::startDataPhase()
{
    DccAppTimeline::instance().log(QStringLiteral("data-phase-start"));
    // 预热：在线程池中提前创建 DSAppletManager 单例（构造即完成 dde-apps
    // applet 初始化），避免首个消费者在插件加载路径上同步承担该开销。
    threadPool()->start(DSAppletManager::instance);
    for (auto &&loader : m_plugins) {
        if ((loader->status() & DccPluginLoader::PluginEnd)
            || !(loader->status() & DccPluginLoader::ModuleEnd)
            || (loader->status() & DccPluginLoader::DataBegin)) {
            continue;
        }

        loader->transitionStatus(DccPluginLoader::DataBegin);
        threadPool()->start(new LoadDataTask(loader, this));
    }
}

void DccPluginManager::checkLoadFinished()
{
    if (m_allLoadFinished || !loadFinished()) {
        return;
    }

    m_allLoadFinished = true;
    m_loadTimer.stop();
    DccAppTimeline::instance().log(QStringLiteral("all-plugins-loaded"));
    Q_EMIT loadAllFinished();
    cancelLoad();
}

void DccPluginManager::cancelLoad()
{
    // Abort any in-flight / queued async QML loading
    m_asyncQueue.clear();
    m_asyncBusy = false;
    for (auto *loader : std::as_const(m_plugins)) {
        loader->cancelAsync();
    }
    if (m_threadPool) {
        qCDebug(dccLog()) << "delete threadPool";
        m_threadPool->clear();
        if (!m_threadPool->waitForDone(3000)) {
            qCWarning(dccLog()) << "thread pool waitForDone timeout, some tasks may still be running";
        }
        delete m_threadPool;
        qCDebug(dccLog()) << "delete threadPool finish";
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

void DccPluginManager::onPluginStatusChanged(DccPluginLoader *loader, uint status)
{
    if (isDeleting()) {
        return;
    }
    if ((status & DccPluginLoader::PluginEndMask)) {
        loadPlugin(loader);
    }
    // Advance the serial async queue when the in-flight loader finished an
    // async stage (module QML or main QML)
    if ((status & (DccPluginLoader::ModuleEnd | DccPluginLoader::MainObjEnd))) {
        m_asyncBusy = false;
        startNextAsync();
    }
}

void DccPluginManager::startNextAsync()
{
    if (m_asyncBusy) {
        return; // a loader is mid-flight
    }
    if (m_asyncQueue.isEmpty()) {
        return;
    }
    m_asyncBusy = true;
    DccPluginLoader *loader = m_asyncQueue.dequeue();
    const auto status = loader->status();
    if ((status & (DccPluginLoader::MetaDataEnd | DccPluginLoader::ModuleLoad))
        == DccPluginLoader::MetaDataEnd) {
        loader->asyncLoadModule();
    } else if ((status & (DccPluginLoader::DataEnd | DccPluginLoader::MainObjLoad))
               == DccPluginLoader::DataEnd) {
        loader->asyncLoadMain();
    } else {
        // 状态不匹配（如中途被 hide 短路等），此 loader 不会再经异步路径发 End，
        // 同步跳过。递归安全：队列单调减小。
        m_asyncBusy = false;
        startNextAsync();
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
