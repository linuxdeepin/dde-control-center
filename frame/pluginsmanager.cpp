#include "pluginsmanager.h"
#include "modulemetadata.h"

#include <QStringList>
#include <QJsonObject>
#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QDebug>

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

void PluginsManager::checkDevices()
{
    // TODO: check device exist.
    m_wacomExist = true;
    m_bluetoothExist = false;
}

bool PluginsManager::checkDeviceDependent(const QString &condition)
{
    qDebug() << condition;

    return true;
}

int PluginsManager::getPluginInsertIndex(const QString &id)
{
    // TODO: make this QStringList dynamic in the future to allow 3rd party modules.
    static QStringList pluginsOrder;
    pluginsOrder << "account" << "display" << "defaultapps" << "personalization";
    pluginsOrder << "network" << "bluetooth" << "sound" << "datetime" << "power";
    pluginsOrder << "mouse" << "wacom" << "keyboard" << "shortcuts" << "grub" << "system_info";

    const int pluginOrder = pluginsOrder.indexOf(id);
    for (const ModuleMetaData &p : m_pluginsList)
        if (pluginsOrder.indexOf(p.id) > pluginOrder)
            return pluginIndex(p.id);

    return m_pluginsList.size();
}

void PluginsManager::insertPlugin(const ModuleMetaData &meta)
{
    const int insertIndex = getPluginInsertIndex(meta.id);

    m_pluginsList.insert(insertIndex, meta);
}

void PluginsManager::loadPlugins()
{
#ifndef QT_DEBUG
    QDir pluginsDir("../lib/dde-control-center/modules");
#else
    QDir pluginsDir("modules");
#endif

    const QStringList files = pluginsDir.entryList(QDir::Files);
    for (QString fileName : files) {
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

        const QString &condition = metaData.value("display_condition").toString();
        if (checkDeviceDependent(condition))
            insertPlugin(meta);
    }
}
