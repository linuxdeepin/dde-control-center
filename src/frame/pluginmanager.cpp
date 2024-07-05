// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pluginmanager.h"

#include "dccfactory.h"
#include "dccmanager.h"
#include "src/interface/dccobject_p.h"

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

Q_LOGGING_CATEGORY(DdcFramePluginManager, "dcc-frame-pluginManager")

#include <mutex>
#include <queue>

std::mutex PLUGIN_LOAD_GUARD;

namespace dccV25 {
static Q_LOGGING_CATEGORY(dccLog, "dde.dcc.plugin")

        const static QString TranslateReadDir = QStringLiteral(TRANSLATE_READ_DIR);

struct PluginData
{
    QString name;
    QString path;
    DccObject *module;
    DccObject *mainObj;
    DccObject *osObj;
    QObject *data;
};

PluginManager::PluginManager(DccManager *parent)
    : QObject(parent)
    , m_manager(parent)
    , m_rootModule(nullptr)
{
    qRegisterMetaType<PluginData>("PluginData");
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

QObject *PluginManager::createObject(const QUrl &url, const QVector<QQmlContext::PropertyPair> &properties)
{
    QQmlEngine *engine = m_manager->engine();
    std::unique_ptr<QQmlComponent> component(new QQmlComponent(engine));
    component->loadUrl(url /*, QQmlComponent::Asynchronous*/);
    if (component->isError()) {
        qCWarning(dccLog()) << "Loading url failed" << component->errorString();
        return nullptr;
    }
    std::unique_ptr<QQmlContext> context(new QQmlContext(engine, engine->rootContext()));
    context->setContextProperties(properties);
    // for(auto property:initialProperties){
    //     context->setContextProperty(property.data())
    // }
    auto object = component->createWithInitialProperties({}, context.get());
    if (!object)
        return nullptr;
    component->completeCreate();
    context.release();
    return object;
}

void PluginManager::loadModules(DccObject *root, bool async, const QStringList &dirs)
{
    if (!root)
        return;
    m_rootModule = root;
    qWarning() << __FUNCTION__ << dirs;
    connect(this, &PluginManager::loadedModule, root, [this](const PluginData &data) {
        initModules(data);
    });

    QFileInfoList pluginList;
    for (const auto &dir : dirs) {
        QDir plugindir(dir);
        if (plugindir.exists()) {
            pluginList += plugindir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        }
    }

    QSet<QString> filenames;
    QList<QPair<PluginManager *, QString>> libraryNames;
    for (auto &lib : pluginList) {
        const QString &filepath = lib.absoluteFilePath();
        auto filename = lib.fileName();
        if (m_manager->hideModule().contains(filename)) {
            // 跳过隐藏的模块,需要动态加载回来
            continue;
        }
        // 不加载文件名重复的模块
        if (filenames.contains(filename)) {
            continue;
        }
        filenames.insert(filename);
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
            qDebug() << "plugin's version is too low";
            continue;
        }
        // {name.qml}
        PluginData *plugin = new PluginData;
        plugin->name = lib.baseName();
        plugin->path = filepath;
        plugin->data = nullptr;
        plugin->module = nullptr;
        plugin->mainObj = nullptr;
        plugin->osObj = nullptr;
        const QString qmlPath = filepath + "/" + lib.baseName() + ".qml";
        if (QFile::exists(qmlPath)) {
            DccObject *obj = qobject_cast<DccObject *>(createObject(qmlPath, {}));
            if (!obj)
                continue;
            plugin->module = obj;
            m_manager->addObject(obj);
        }
        // {main.qml}  可放线程中
        const QString mainPath = filepath + "/main.qml";
        const QString osPath = filepath + "/" + lib.baseName() + ".so";
        QObject *dataObj = nullptr;
        DccObject *osObj = nullptr;
        if (QFile::exists(osPath)) {
            QPluginLoader loader(osPath);
            loader.load();
            if (!loader.isLoaded()) {
                qCWarning(dccLog) << "Load the plugin failed." << loader.errorString();
            } else {
                const auto &meta = loader.metaData();
                do {
                    const auto iid = meta["IID"].toString();
                    if (iid.isEmpty())
                        break;

                    if (iid != QString(qobject_interface_iid<DccFactory *>()))
                        break;

                    if (!loader.instance()) {
                        qWarning(dccLog) << "Load the plugin failed." << loader.errorString();
                        break;
                    }
                    DccFactory *factory = qobject_cast<DccFactory *>(loader.instance());
                    if (!factory) {
                        qWarning(dccLog) << "The plugin isn't a DccFactory." << osPath;
                        break;
                    }
                    dataObj = factory->create(parent());
                    osObj = factory->dccObject(parent());
                } while (false);
            }
        } else {
            qCInfo(dccLog) << "File does not exist:" << osPath;
        }
        if (dataObj) {
            plugin->data = dataObj;
        }
        if (osObj) {
            plugin->osObj = osObj;
        }
        if (QFile::exists(mainPath)) {
            DccObject *obj = qobject_cast<DccObject *>(createObject(mainPath, { { "dccData", QVariant::fromValue(plugin->data) } }));
            if (!obj)
                continue;
            plugin->mainObj = obj;
        }
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
            Q_EMIT addObject(plugin->mainObj);
            Q_EMIT addObject(plugin->osObj);
        } else {
            qWarning(dccLog) << "The plugin isn't main DccObject" << osPath;
        }

        m_plugins.append(plugin);
    }
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

DccObject *PluginManager::findModule(DccObject *module, const QString &name)
{
    if (!module)
        return nullptr;

    return nullptr;
}

void PluginManager::initModules(const PluginData &data) { }

void PluginManager::insertChild(bool force) { }
}; // namespace dccV25
