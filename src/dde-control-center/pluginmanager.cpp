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
    } else if ((loader->status() & (DccPluginLoader::DataEnd | DccPluginLoader::MainObjLoad)) == DccPluginLoader::DataEnd) {
        loader->transitionStatus(DccPluginLoader::MainObjLoad);
        loader->createDccObject();
        loader->updateParent();
        loader->loadMain();
        loader->transitionStatus(DccPluginLoader::MainObjEnd);
    } else if ((loader->status() & (DccPluginLoader::ModuleEnd | DccPluginLoader::DataBegin)) == DccPluginLoader::ModuleEnd) {
        loader->transitionStatus(DccPluginLoader::DataBegin);
        if (loader->module()) {
            Q_EMIT addObject(loader->module());
        }
        threadPool()->start(new LoadDataTask(loader, this));
    } else if ((loader->status() & (DccPluginLoader::MetaDataEnd | DccPluginLoader::ModuleLoad)) == DccPluginLoader::MetaDataEnd) {
        loader->transitionStatus(DccPluginLoader::ModuleLoad);
        if (loader->loadModule()) {
            loader->transitionStatus(DccPluginLoader::ModuleEnd);
        } else {
            loader->transitionStatus(DccPluginLoader::ModuleEnd | DccPluginLoader::PluginEnd);
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
        m_threadPool->waitForDone();
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
