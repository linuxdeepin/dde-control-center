// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pluginmanager.h"

#include "dccfactory.h"
#include "dccmanager.h"
// #include "src/interface/dccobject_p.h"

#include <DGuiApplicationHelper>
#include <DNotifySender>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QLoggingCategory>
#include <QPair>
#include <QPluginLoader>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QSet>
#include <QSettings>
#include <QtConcurrent>
#include <QtConcurrentRun>

namespace dccV25 {
static Q_LOGGING_CATEGORY(dccLog, "dde.dcc.plugin");

const static QString TranslateReadDir = QStringLiteral(TRANSLATE_READ_DIR);

enum PluginStatus {
    // metaData 0xFF000000
    PluginBegin = 0x01000000,
    PluginEnd = 0x02000000,
    MetaDataErr = 0x04000000,
    // module 0x00FF0000
    ModuleLoad = 0x00010000,
    ModuleCreate = 0x00020000,
    ModuleEnd = 0x00400000,
    ModuleErr = 0x00800000,
    // data 0x0000FF00
    DataBegin = 0x00000100,
    DataLoad = 0x00000300,
    DataEnd = 0x00004000,
    DataErr = 0x00008000,
    // mainObj 0x000000FF
    MainObjLoad = 0x00000001,
    MainObjCreate = 0x00000002,
    MainObjAdd = 0x00000004,
    MainObjEnd = 0x00000040,
    MainObjErr = 0x00000080,
};

struct PluginData
{
    QString name;
    QString path;
    DccObject *module;
    DccObject *mainObj;
    DccObject *osObj;
    QObject *data;
    uint status;

    PluginData(const QString &_name, const QString &_path)
        : name(_name)
        , path(_path)
        , module(nullptr)
        , mainObj(nullptr)
        , osObj(nullptr)
        , data(nullptr)
        , status(PluginBegin)
    {
    }
};

PluginManager::PluginManager(DccManager *parent)
    : QObject(parent)
    , m_manager(parent)
    , m_rootModule(nullptr)
{
    qRegisterMetaType<PluginData>("PluginData");
    connect(this, &PluginManager::loadOsFinished, this, &PluginManager::loadMain, Qt::QueuedConnection);
}

bool PluginManager::compareVersion(const QString &targetVersion, const QString &baseVersion)
{
    QStringList version1 = baseVersion.split(".");
    QStringList version2 = targetVersion.split(".");

    if (version1.size() != version2.size()) {
        return false;
    }

    for (int i = 0; i < version1.size(); ++i) {
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

void PluginManager::loadModule(PluginData *plugin)
{
    const QString qmlPath = plugin->path + "/" + plugin->name + ".qml";
    qCInfo(dccLog()) << plugin->name << ": status" << QString::number(plugin->status, 16) << ": load module" << qmlPath;
    plugin->status |= ModuleLoad;
    if (QFile::exists(qmlPath)) {
        QQmlComponent *component = new QQmlComponent(m_manager->engine());
        component->setProperty("PluginData", QVariant::fromValue(plugin));
        component->loadUrl(qmlPath, QQmlComponent::Asynchronous);
        if (component->isLoading())
            connect(component, &QQmlComponent::statusChanged, this, &PluginManager::moduleLoading);
        else
            createModule(component);
    }
}

void PluginManager::loadMain(PluginData *plugin)
{
    const QString qmlPath = plugin->path + "/main.qml";
    qCInfo(dccLog()) << plugin->name << ": status" << QString::number(plugin->status, 16) << ":load main";
    plugin->status |= MainObjLoad;
    if (QFile::exists(qmlPath)) {
        QQmlComponent *component = new QQmlComponent(m_manager->engine());
        component->setProperty("PluginData", QVariant::fromValue(plugin));
        component->loadUrl(qmlPath, QQmlComponent::Asynchronous);
        if (component->isLoading())
            connect(component, &QQmlComponent::statusChanged, this, &PluginManager::mainLoading);
        else
            createMain(component);
    } else {
        addMainObject(plugin);
    }
}

void PluginManager::createModule(QQmlComponent *component)
{
    PluginData *plugin = component->property("PluginData").value<PluginData *>();
    qCInfo(dccLog()) << plugin->name << ": status" << QString::number(plugin->status, 16) << ": create module";
    plugin->status |= ModuleCreate;
    if (component->isError()) {
        qCWarning(dccLog()) << plugin->name << " component create module object error:" << component->errors();
    } else {
        QObject *object = component->create();
        if (!object) {
            plugin->status |= ModuleErr;
            qCWarning(dccLog()) << plugin->name << " component create module object is null:" << component->errors();
            return;
        }
        plugin->module = qobject_cast<DccObject *>(object);
        qCInfo(dccLog()) << plugin->name << ": status" << QString::number(plugin->status, 16) << ": create module finished";
        plugin->status |= ModuleEnd;
        m_manager->addObject(plugin->module);
    }
}

void PluginManager::createMain(QQmlComponent *component)
{
    PluginData *plugin = component->property("PluginData").value<PluginData *>();
    qCInfo(dccLog()) << plugin->name << ": status" << QString::number(plugin->status, 16) << ": create main";
    plugin->status |= MainObjCreate;
    if (component->isError()) {
        qCWarning(dccLog()) << plugin->name << " component create main object error:" << component->errors();
    } else {
        QQmlContext *context = new QQmlContext(component->engine(), component);
        context->setContextProperties({ { "dccData", QVariant::fromValue(plugin->data) } });
        QObject *object = component->create(context);
        // component->createWithInitialProperties({}, context);
        if (!object) {
            plugin->status |= MainObjErr;
            qCWarning(dccLog()) << plugin->name << " component create main object is null:" << component->errors();
            return;
        }
        qCInfo(dccLog()) << plugin->name << ": status" << QString::number(plugin->status, 16) << ": create main finished";
        plugin->mainObj = qobject_cast<DccObject *>(object);
    }
    addMainObject(plugin);
}

void PluginManager::addMainObject(PluginData *plugin)
{
    plugin->status |= MainObjAdd;
    qCInfo(dccLog()) << plugin->name << ": status" << QString::number(plugin->status, 16) << ": add main object";
    if (!plugin->mainObj) {
        plugin->mainObj = plugin->osObj;
    }
    if (plugin->mainObj) {
        if (plugin->mainObj->name().isEmpty() || (plugin->module && plugin->mainObj->name() == plugin->module->name())) {
            // 插件根项name为空时，关联{name}.qml,不加树
            if (plugin->module) {
                plugin->module->setPage(plugin->mainObj->page());
                connect(plugin->mainObj, &DccObject::pageChanged, plugin->module, &DccObject::setPage);
                connect(plugin->mainObj, &DccObject::displayNameChanged, plugin->module, &DccObject::setDisplayName);
                connect(plugin->mainObj, &DccObject::descriptionChanged, plugin->module, &DccObject::setDescription);
                connect(plugin->mainObj, &DccObject::iconChanged, plugin->module, &DccObject::setIcon);
                connect(plugin->mainObj, &DccObject::badgeChanged, plugin->module, &DccObject::setBadge);
            }
        } else {
        }
    } else {
        plugin->status |= MainObjErr;
        qWarning(dccLog) << "The plugin isn't main DccObject" << plugin->name;
    }
    plugin->status |= MainObjEnd;
    plugin->status |= PluginEnd;
    if (plugin->mainObj) {
        qCInfo(dccLog()) << plugin->name << ": status" << QString::number(plugin->status, 16) << ": add main object finished";
        Q_EMIT addObject(plugin->mainObj);
    }
    if (plugin->osObj) {
        Q_EMIT addObject(plugin->osObj);
    }
}

void PluginManager::moduleLoading()
{
    QQmlComponent *component = qobject_cast<QQmlComponent *>(sender());
    if (component)
        createModule(component);
}

void PluginManager::mainLoading()
{
    QQmlComponent *component = qobject_cast<QQmlComponent *>(sender());
    if (component)
        createMain(component);
}

void loadPlugin(const QPair<PluginManager *, PluginData *> &pair)
{
    PluginManager *self = pair.first;
    PluginData *plugin = pair.second;
    plugin->status |= DataBegin;
    qCInfo(dccLog()) << plugin->name << ": status" << QString::number(plugin->status, 16) << ": load plugin begin";
    // {main.qml}  可放线程中
    const QString osPath = plugin->path + "/" + plugin->name + ".so";
    QElapsedTimer timer;
    timer.start();
    QObject *dataObj = nullptr;
    DccObject *osObj = nullptr;
    if (QFile::exists(osPath)) {
        QPluginLoader loader(osPath);
        plugin->status |= DataLoad;
        loader.load();
        if (!loader.isLoaded()) {
            plugin->status |= DataErr;
            qCWarning(dccLog) << "Load the plugin failed." << loader.errorString();
        } else {
            const auto &meta = loader.metaData();
            do {
                const auto iid = meta["IID"].toString();
                if (iid.isEmpty() || iid != QString(qobject_interface_iid<DccFactory *>())) {
                    plugin->status |= DataErr;
                    break;
                }

                if (!loader.instance()) {
                    plugin->status |= DataErr;
                    qWarning(dccLog) << "Load the plugin failed." << loader.errorString();
                    break;
                }
                DccFactory *factory = qobject_cast<DccFactory *>(loader.instance());
                if (!factory) {
                    plugin->status |= DataErr;
                    qWarning(dccLog) << "The plugin isn't a DccFactory." << osPath;
                    break;
                }
                dataObj = factory->create();
                osObj = factory->dccObject();
            } while (false);
        }
    } else {
        plugin->status |= DataErr;
        qCInfo(dccLog) << "File does not exist:" << osPath;
    }
    if (dataObj) {
        plugin->data = dataObj;
    }
    if (osObj) {
        plugin->osObj = osObj;
    }
    if (plugin->data) {
        plugin->data->moveToThread(self->thread());
        plugin->data->setParent(self->parent());
    }
    if (plugin->osObj) {
        plugin->osObj->moveToThread(self->thread());
        plugin->osObj->setParent(self->parent());
    }
    Q_EMIT self->loadOsFinished(plugin);
    plugin->status |= DataEnd;
    qCInfo(dccLog()) << plugin->name << ": status" << QString::number(plugin->status, 16) << ": load plugin finished. elasped time :" << timer.elapsed();
}

void PluginManager::loadModules(DccObject *root, bool async, const QStringList &dirs)
{
    if (!root)
        return;
    m_rootModule = root;
    qCDebug(dccLog()) << "plugin dir:" << dirs;

    QFileInfoList pluginList;
    for (const auto &dir : dirs) {
        QDir plugindir(dir);
        if (plugindir.exists()) {
            pluginList += plugindir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        }
    }

    QList<QPair<PluginManager *, PluginData *>> libraryList;
    for (auto &lib : pluginList) {
        const QString &filepath = lib.absoluteFilePath();
        auto filename = lib.fileName();
        if (m_manager->hideModule().contains(filename)) {
            // 跳过隐藏的模块,需要动态加载回来
            continue;
        }
        // metadata
        const QString metadataPath = filepath + "/metadata.json";
        QFile metadataFile(metadataPath);
        if (!metadataFile.open(QIODevice::ReadOnly)) {
            qCWarning(dccLog) << "Couldn't open" << metadataFile.fileName();
            continue;
        }
        QJsonParseError error;
        const QJsonObject metaData = QJsonDocument::fromJson(metadataFile.readAll()).object();
        metadataFile.close();
        if (error.error) {
            qCWarning(dccLog) << "error parsing json data" << error.errorString();
            continue;
        }
        QString version = metaData.value("Version").toString();
        if (!compareVersion(metaData.value("Version").toString(), "1.0")) {
            qCDebug(dccLog()) << "plugin's version is too low";
            continue;
        }
        PluginData *plugin = new PluginData(lib.baseName(), filepath);
        loadModule(plugin);
        libraryList.append({ this, plugin });
        m_plugins.append(plugin);
    }
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    m_future = QtConcurrent::map(libraryList, loadPlugin);
    connect(watcher, &QFutureWatcher<PluginData>::finished, this, [this] {
        qCInfo(dccLog()) << "load all finished";
        emit loadAllFinished();
    });
    watcher->setFuture(m_future);
}

void PluginManager::cancelLoad()
{
    if (!loadFinished()) {
        m_future.cancel();
    }
}

bool PluginManager::loadFinished() const
{
    return m_pluginsStatus.isEmpty();
}

}; // namespace dccV25
Q_DECLARE_METATYPE(dccV25::PluginData *)
