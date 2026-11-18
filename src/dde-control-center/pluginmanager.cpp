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

namespace dccV25 {

const static QString TranslateReadDir = QStringLiteral(TRANSLATE_READ_DIR);

// 工作线程：加载动态库并识别接口版本。
// v1 在同一任务里完成 create/moveThread；v2 只做到把 factory 迁到主线程，
// 由主线程执行 initialized() 后再调度 CreateDataTask。
class LoadFactoryTask : public QRunnable
{
public:
    explicit LoadFactoryTask(DccPluginLoader *loader, DccPluginManager *manager)
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
        const bool needInitialize = m_loader->loadData();
        m_loader->transitionStatus(DccPluginLoader::DataLoad);
        if (m_manager->isDeleting()) {
            return;
        }
        if (needInitialize) {
            Q_EMIT m_loader->factoryReady(m_loader);
            return;
        }
        // 纯 QML 插件（无 {name}.so）：没有数据后端，跳过 createData()/moveThread()，
        // 避免无工厂路径设置 DataErr、把正常插件误判为损坏后端。
        if (m_loader->hasDataBackend()) {
            // createData和moveThread需要完整，都执行或都不执行
            m_loader->createData();
            m_loader->moveThread();
        }
        m_loader->transitionStatus(DccPluginLoader::DataEnd);
    }

private:
    DccPluginLoader *m_loader;
    DccPluginManager *m_manager;
};

// 工作线程：v2 在 initialized() 之后创建数据对象并迁移回主线程
class CreateDataTask : public QRunnable
{
public:
    explicit CreateDataTask(DccPluginLoader *loader, DccPluginManager *manager)
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
        // createData 与 moveThread 必须成对执行，不能在这里插入取消返回
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
    , m_showOnNavigationReady(false)
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
        // 走到 PluginEnd 却从未进入数据阶段（错误、隐藏、无 .so）：
        // 明确收束为一个终态，排队中的请求才能立即得到结果而不是等超时。
        // 只有 DataErr 才代表后端损坏；页面/元数据错误属于插件包问题，
        // 后端本身仍应报 NotSupported，而不是 "backend failed to load"。
        if (loader->backendState() == DccPluginLoader::BackendState::NotDiscovered
                || loader->backendState() == DccPluginLoader::BackendState::Loading) {
            loader->setBackendState(loader->hasDataError()
                                    ? DccPluginLoader::BackendState::Failed
                                    : DccPluginLoader::BackendState::NotSupported);
        }
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
        loader->transitionStatus(DccPluginLoader::MainObjLoad);
        loader->updateParent();
        // 后端必须在依赖它的页面创建之前激活
        loader->activate();
        loader->createDccObject();
        loader->updateParent();
        loader->loadMain();
        loader->transitionStatus(DccPluginLoader::MainObjEnd);
        // 后端进入终态：有工厂且至少支持一种数据操作即为 Ready，
        // 具体操作由请求层按 Get/Set 能力位分别判定
        if (loader->factory() && (loader->supportsCapability(DccCapability::Get)
                                  || loader->supportsCapability(DccCapability::Set))) {
            loader->setBackendState(DccPluginLoader::BackendState::Ready);
        } else if (loader->hasDataError()) {
            // 后端自身加载失败（.so 打不开、IID 不符、转换失败）：必须报 Failed。
            // 页面的 MainObjErr 不算——页面失败不影响"是否提供数据能力"的判定，
            // 否则纯 QML 的页面错误会被误报成后端损坏。
            loader->setBackendState(DccPluginLoader::BackendState::Failed);
        } else {
            // 纯 QML 或 v1：没有可服务的数据接口
            loader->setBackendState(DccPluginLoader::BackendState::NotSupported);
        }
        // v2 的变化通知由框架补充插件 ID 后转发。
        // 注意：lambda 作槽不支持 Qt::UniqueConnection（Qt 会断言），
        // 因此这里用成员函数指针，并由 connectModuleSignals 做一次性登记。
        connectFactorySignals(loader);
    } else if ((loader->status() & (DccPluginLoader::ModuleEnd | DccPluginLoader::DataBegin)) == DccPluginLoader::ModuleEnd) {
        // 模块已就绪但数据阶段尚未为它启动：补跑数据阶段。
        // 加载时被判为隐藏、之后才可见的插件依赖这里重新纳入 startDataPhase()。
        checkNavigationFinished();
    } else if ((loader->status() & (DccPluginLoader::MetaDataEnd | DccPluginLoader::ModuleLoad)) == DccPluginLoader::MetaDataEnd) {
        loader->transitionStatus(DccPluginLoader::ModuleLoad);
        const auto ret = loader->loadModule();
        if (auto module = loader->module()) {
            Q_EMIT addObject(module);
        }
        if (ret) {
            loader->transitionStatus(DccPluginLoader::ModuleEnd);
            Q_EMIT moduleLoaded(loader->name());
        } else {
            loader->transitionStatus(DccPluginLoader::ModuleEnd | DccPluginLoader::PluginEnd);
            loader->setBackendState(DccPluginLoader::BackendState::NotSupported);
        }
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
        connect(loader, &DccPluginLoader::factoryReady, this, &DccPluginManager::onFactoryReady, Qt::QueuedConnection);
        // loader 的后端终态直接转发给 manager，避免再经由状态位推断
        connect(loader, &DccPluginLoader::backendStateChanged, this, &DccPluginManager::backendStateChanged, Qt::QueuedConnection);

        loaders.append(loader);
    }

    for (const auto &plugin : m_pluginsToLoad) {
        if (!matchedPlugins.contains(plugin)) {
            qCWarning(dccLog()) << "Requested plugin was not found:" << plugin;
        }
    }

    m_plugins = loaders;
    m_navigationFinished = false;
    m_dataPhaseStarted = false;
    m_allLoadFinished = false;

    // Start loading all plugins
    for (auto &&loader : m_plugins) {
        loadPlugin(loader);
    }
    checkNavigationFinished();
}

void DccPluginManager::checkNavigationFinished()
{
    bool hasReadyPlugin = false;
    for (auto &&loader : m_plugins) {
        const auto status = loader->status();
        if (!(status & (DccPluginLoader::ModuleEnd | DccPluginLoader::PluginEnd))) {
            return;
        }
        if ((status & (DccPluginLoader::ModuleEnd | DccPluginLoader::DataBegin | DccPluginLoader::PluginEnd)) == DccPluginLoader::ModuleEnd) {
            hasReadyPlugin = true;
        }
    }
    if (m_navigationFinished) {
        if (m_dataPhaseStarted && hasReadyPlugin) {
            startDataPhase();
        }
    } else {
        m_navigationFinished = true;
        DccAppTimeline::instance().log(QStringLiteral("navigation-ready"));
        // 发现阶段结束必须立即通知请求层：它需要把"找不到 loader"的
        // 排队请求收束为 NotFound。这里用直连之外的专用信号，
        // 不依赖 navigationReady 的排队时序（后者可能被 UI 流程延后）。
        Q_EMIT discoveryFinishedChanged();
        Q_EMIT navigationReady();
        if (showOnNavigationReady()) {
            QTimer::singleShot(100, this, &DccPluginManager::startDataPhase);
        } else {
            startDataPhase();
        }
    }
}

void DccPluginManager::startDataPhase()
{
    if (!m_dataPhaseStarted) {
        m_dataPhaseStarted = true;
        DccAppTimeline::instance().log(QStringLiteral("data-phase-start"));
        // 预热：在线程池中提前创建 DSAppletManager 单例（构造即完成 dde-apps
        // applet 初始化），避免首个消费者在插件加载路径上同步承担该开销。
        threadPool()->start(DSAppletManager::instance);
    }
    for (auto &&loader : m_plugins) {
        if ((loader->status() & DccPluginLoader::PluginEnd)
            || !(loader->status() & DccPluginLoader::ModuleEnd)
            || (loader->status() & DccPluginLoader::DataBegin)) {
            continue;
        }

        loader->transitionStatus(DccPluginLoader::DataBegin);
        threadPool()->start(new LoadFactoryTask(loader, this));
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
    const bool wasDeleting = m_isDeleting.exchange(true);
    if (wasDeleting) {
        return;
    }
    // 先停用后端，确保其后端任务、定时器与外部通知在 engine/模块树释放前收束。
    // 必须在主线程执行；此时 loader 的数据对象仍然存活。
    for (auto &&loader : m_plugins) {
        if (loader) {
            loader->deactivate();
        }
    }
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

void DccPluginManager::setShowOnNavigationReady(bool enabled)
{
    m_showOnNavigationReady = enabled;
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

void DccPluginManager::connectFactorySignals(DccPluginLoader *loader)
{
    DccFactory_v20 *factory = loader ? loader->factory() : nullptr;
    if (!factory || m_connectedFactories.contains(factory)) {
        return;
    }
    // 未声明变更通知能力的工厂不连接
    if (!factory->capabilities().testFlag(DccCapability::PropertiesChanged)) {
        return;
    }
    m_connectedFactories.insert(factory);
    // 成员函数指针作槽，允许使用 UniqueConnection；同时用集合防重复登记
    connect(factory, &DccFactory_v20::propertiesChanged, this, &DccPluginManager::onFactoryPropertiesChanged, Qt::UniqueConnection);
}

void DccPluginManager::onFactoryPropertiesChanged(const QVariantMap &properties)
{
    auto *factory = qobject_cast<DccFactory_v20 *>(sender());
    if (!factory) {
        return;
    }
    // 插件 ID 由 loader 名称决定，逐个匹配已连接的工厂
    for (auto &&loader : m_plugins) {
        if (loader && loader->factory() == factory) {
            Q_EMIT modulePropertiesChanged(loader->name(), properties);
            return;
        }
    }
}

DccPluginLoader *DccPluginManager::findLoader(const QString &module) const
{
    DccPluginLoader *found = nullptr;
    for (auto &&loader : m_plugins) {
        if (!loader || loader->name() != module) {
            continue;
        }
        if (found) {
            // 同名 loader 不任取一个：调用方会得到明确的歧义错误
            qCWarning(dccLog()) << "duplicate loader for module:" << module;
            return nullptr;
        }
        found = loader;
    }
    return found;
}

bool DccPluginManager::discoveryFinished() const
{
    return m_navigationFinished;
}

void DccPluginManager::onFactoryReady(DccPluginLoader *loader)
{
    if (isDeleting() || !loader) {
        return;
    }
    // 仅 v2 会发该信号：初始化在主线程执行，早于 create
    loader->initializeFactory();
    threadPool()->start(new CreateDataTask(loader, this));
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
