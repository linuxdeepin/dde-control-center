#include "pluginmanager.h"
#include "layout/layoutmanager.h"
#include "interface/moduleobject.h"
#include "interface/plugininterface.h"

#include <QDir>
#include <QDebug>
#include <QElapsedTimer>
#include <QPluginLoader>
#include <QCoreApplication>
#include <QtConcurrent>
#include <queue>

DCC_USE_NAMESPACE

const QString &PluginDirectory = QStringLiteral("/usr/lib/dde-control-center/modules");

bool compareVersion(const QString &targetVersion, const QString &baseVersion)
{
    const QStringList &version1 = baseVersion.split(".");
    const QStringList &version2 = targetVersion.split(".");

    const auto size = qAbs<int>(version1.size() - version2.size());

    for (int i = 0; i < size; ++i) {
        // 相等判断下一个子版本号
        if (version1[i] == version2[i])
            continue;

        // 转成整形比较
        if (version1[i].toInt() > version2[i].toInt()) {
            return false;
        } else {
            return true;
        }
    }

    return true;
}

bool comparePluginLocation(PluginInterface const* target1, PluginInterface const* target2)
{
    return target1->location() < target2->location();
}

PluginData loadModule(const QPair<PluginManager*,QString> &pair)
{
    PluginData data;
    data.Module = nullptr;
    auto &&fileName = pair.second;
    qInfo() << "loading plugin: " << fileName;

    QElapsedTimer et;
    et.start();
    QPluginLoader *loader = new QPluginLoader(fileName);
    if (!loader->load()) {
        qWarning() << QString("The plugin: %1 load failed! error message: %2").arg(fileName, loader->errorString());
        return data;
    }
    const QJsonObject &meta = loader->metaData().value("MetaData").toObject();
    if (!compareVersion(meta.value("api").toString(), "1.0.0")) {
        qWarning() << QString("The version of plugin: %1 is too low!").arg(fileName);
        return data;
    }

    PluginInterface * plugin = qobject_cast<PluginInterface *>(loader->instance());
    if (!plugin) {
        qWarning() << QString("Can't read plugin: %1").arg(fileName);
        loader->unload();
        return data;
    }

    data.Module = plugin->module();
    data.Follow = plugin->follow();
    data.Location = plugin->location();
    data.layoutFactory = plugin->layoutFactory();

    data.Module->setParent(nullptr);
    data.Module->moveToThread(qApp->thread());
    emit pair.first->loadedModule(data);
    qInfo() << QString("load plugin: %1 end, using time: %2 ms").arg(fileName).arg(et.elapsed());
    return data;
}


PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
    , m_rootModule(nullptr)
{
    qRegisterMetaType<PluginData>("PluginData");
}

void PluginManager::loadModules(ModuleObject *root, LayoutManager *layoutManager)
{
    if (!root || !layoutManager)
        return;
    m_rootModule = root;
    m_layoutManager = layoutManager;

    connect(this, &PluginManager::loadedModule, root, [this] (const PluginData &data) {
        initModules(data);
    });

    QDir pluginDir(PluginDirectory);
#ifdef QT_DEBUG
    pluginDir.setPath(qApp->applicationDirPath());
#endif
    if (!pluginDir.exists()) {
        qWarning() << "plugin directory not exists";
        return;
    }

    auto &&pluginList = pluginDir.entryInfoList();
    QList<QPair<PluginManager*,QString>> libraryNames;
    for (auto &&lib : pluginList) {
        auto &&fileName = lib.absoluteFilePath();
        if (!QLibrary::isLibrary(fileName))
            continue;
        libraryNames.append({this,fileName});
    }

    QFutureWatcher<PluginData> *watcher= new QFutureWatcher<PluginData>(this);
    QFuture<PluginData> future = QtConcurrent::mapped(libraryNames, loadModule);
    connect(watcher, &QFutureWatcher<PluginData>::finished, this, [this] {
        // 加载非一级插件
        for (auto &&data : m_datas) {
            auto &&module = findModule(m_rootModule, data.Follow);
            if (module) {
                module->insertChild(data.Location, data.Module);
            }
        }
    });
    watcher->setFuture(future);
}

ModuleObject *PluginManager::findModule(ModuleObject *module, const QString &name)
{
    if (!module)
        return nullptr;

    std::queue<ModuleObject*> qbfs;
    qbfs.push(module);
    while (!qbfs.empty()) {
        if (qbfs.front()->name() == name)
            return qbfs.front();
        for (auto &&child : qbfs.front()->childrens()) {
            qbfs.push(child);
        }
        qbfs.pop();
    }

    return nullptr;
}

void PluginManager::initModules(const PluginData &data)
{
    m_layoutManager->registerLayout(data.layoutFactory);
    if (data.Follow.isEmpty()) {    // root plugin
        data.Module->setProperty("location", data.Location);
        if (!m_rootModule->hasChildrens()) {
            m_rootModule->appendChild(data.Module);
            return;
        }
        if (data.Location < 0) {
            m_rootModule->appendChild(data.Module);
            return;
        }
        int i = m_rootModule->childrens().count() - 1;
        for (; i >= 0; i--) {
            auto &&tmpLocation = m_rootModule->childrens().at(i)->property("location").toInt();
            if (tmpLocation >= 0 && data.Location > tmpLocation) {
                break;
            }
        }
        m_rootModule->insertChild(i + 1, data.Module);
    } else {    // other plugin
        m_datas.append(data);
    }
}
