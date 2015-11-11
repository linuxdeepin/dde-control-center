#include "pluginsmanager.h"
#include "modulemetadata.h"

#include <QStringList>
#include <QJsonObject>
#include <QDir>
#include <QLibrary>
#include <QPluginLoader>

static PluginsManager *m_self = nullptr;

PluginsManager *PluginsManager::getInstance(QObject *parent)
{
    if (!m_self)
        m_self = new PluginsManager(parent);

    return m_self;
}

int PluginsManager::pluginIndex(const QString pluginId) const
{
    const int count = m_pluginsList.count();
    for (int i(0); i != count; ++i)
        if (m_pluginsList.at(i).id == pluginId)
            return i;
    return -1;
}

int PluginsManager::pluginIndex(const ModuleMetaData &plugin) const
{
    return pluginIndex(plugin.id);
}

const QString PluginsManager::pluginPath(const QString pluginId) const
{
    int index(pluginIndex(pluginId));

    if (index != -1)
        return m_pluginsList.at(index).path;

    return QString();
}

const QList<ModuleMetaData> &&PluginsManager::pluginsList() const
{
    return std::move(m_pluginsList);
}

PluginsManager::PluginsManager(QObject *parent)
    : QObject(parent)
{
    loadPlugins();
}

void PluginsManager::loadPlugins()
{
#ifndef QT_DEBUG
    QDir pluginsDir("../lib/dde-control-center/modules");
#else
    QDir pluginsDir("modules");
#endif

    // TODO: make this QStringList dynamic in the future to allow 3rd party modules.
    QStringList moduleOrder;
    moduleOrder << "account" << "display" << "defaultapps" << "personalization";
    moduleOrder << "network" << "bluetooth" << "sound" << "datetime" << "power";
    moduleOrder << "mouse" << "wacom" << "keyboard" << "shortcuts" << "grub" << "system_info";

    foreach(QString fileName, pluginsDir.entryList(QDir::Files)) {
        if (!QLibrary::isLibrary(fileName)) {
            continue;
        }
        QString filePath = pluginsDir.absoluteFilePath(fileName);
        QPluginLoader pluginLoader(filePath);
        QJsonObject metaData = pluginLoader.metaData().value("MetaData").toObject();

        ModuleMetaData meta = {
            filePath,
            metaData.value("id").toString(),
            metaData.value("name").toString()
        };

        m_pluginsList << meta;
    }

    qSort(m_pluginsList.begin(), m_pluginsList.end(), [&](const ModuleMetaData & data1, const ModuleMetaData & data2) {
        return moduleOrder.indexOf(data1.id) < moduleOrder.indexOf(data2.id);
    });
}
