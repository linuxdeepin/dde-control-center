#include "pluginmanager.h"
#include "interface/moduleobject.h"
#include "interface/plugininterface.h"

#include <QDir>
#include <QDebug>
#include <QElapsedTimer>
#include <QPluginLoader>
#include <QCoreApplication>

DCC_USE_NAMESPACE

const QString &PluginDirectory = QStringLiteral("/usr/lib/dde-control-center/modules");

PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
    , m_rootModule(new ModuleObject(this))
{

}

void PluginManager::loadModules()
{
    QDir pluginDir(PluginDirectory);
#ifdef QT_DEBUG
    pluginDir.setPath(qApp->applicationDirPath());
#endif
    if (!pluginDir.exists()) {
        qWarning() << "plugin directory not exists";
        return;
    }

    const auto &pluginList = pluginDir.entryInfoList();
    for (auto pluginName : pluginList) {
        QString path = pluginName.absoluteFilePath();

        if (!QLibrary::isLibrary(path))
            continue;

        qInfo() << "loading plugin: " << path;

        QElapsedTimer et;
        et.start();
        QPluginLoader *loader = new QPluginLoader(path, this);
        if (!loader->load()) {
            qWarning() << QString("The plugin: %1 load failed! error message: %2").arg(path).arg(loader->errorString());
        }
        const QJsonObject &meta = loader->metaData().value("MetaData").toObject();
        if (!compareVersion(meta.value("api").toString(), "1.0.0")) {
            qWarning() << QString("The version of plugin: %1 is too low!").arg(path);
            continue;
        }

        PluginInterface * plugin = qobject_cast<PluginInterface *>(loader->instance());
        if (!plugin) {
            qWarning() << QString("Can't read plugin: %1").arg(path);
            loader->unload();
            continue;
        }
        qInfo() << QString("load plugin: %1 end, using time: %2 ms").arg(path).arg(et.elapsed());
        // 防止内存泄露
        plugin->setParent(this);
        m_loaders.append(loader);
    }
    initModules();
}

void PluginManager::initModules()
{
    QMap<QString, ModuleObject*> modules;
    QList<QPluginLoader *> secondLoaders;
    for (auto loader : m_loaders) {
        auto *plugin = qobject_cast<PluginInterface *>(loader->instance());
        if (plugin->follow().isEmpty()) {
            modules[plugin->module()->name()] = plugin->module();
            m_rootModule->appendChild(plugin->module());
        } else {
            secondLoaders.append(loader);
        }
    }
    for (auto loader : secondLoaders) {
        auto *plugin = qobject_cast<PluginInterface *>(loader->instance());
        if (modules.contains(plugin->follow())) {
            modules[plugin->follow()]->appendChild(plugin->module());
        } else {
            qWarning() << QString("can't find follow:%1, the plugin:%2 will unload!").arg(plugin->follow()).arg(plugin->name());
            loader->unload();
        }
    }

}

bool PluginManager::compareVersion(const QString &targetVersion, const QString &baseVersion)
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
