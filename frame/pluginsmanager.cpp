#include "pluginsmanager.h"
#include "modulemetadata.h"

#include <QStringList>
#include <QJsonObject>
#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QDebug>
#include <QDBusInterface>
#include <QDBusReply>

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

    m_deviceMoniter = new DeviceMoniter(this);

    connect(m_deviceMoniter, &DeviceMoniter::deviceChanged, this, &PluginsManager::loadPlugins);

    // set low priority
    m_deviceMoniter->start(QThread::IdlePriority);
}

bool PluginsManager::checkDependentCondition(const QString &condition)
{
    if (condition.isEmpty())
        return true;
    if (!m_deviceMoniter)
        return false;

    return m_deviceMoniter->property(condition.toStdString().c_str()).toBool();
}

int PluginsManager::getPluginInsertIndex(const QString &id)
{
    // TODO: make this QStringList dynamic in the future to allow 3rd party modules.
    static QStringList pluginsOrder({"account", "display", "defaultapps", "personalization",
                                     "network", "bluetooth", "sound", "datetime",
                                     "power", "mouse", "wacom", "keyboard", "shortcuts",
                                     "grub", "remoting", "system_info"});

    const int pluginOrder = pluginsOrder.indexOf(id);
    for (const ModuleMetaData &p : m_pluginsList)
        if (pluginsOrder.indexOf(p.id) > pluginOrder)
            return pluginIndex(p.id);

    return m_pluginsList.size();
}

void PluginsManager::insertPlugin(const ModuleMetaData &meta)
{
    if (pluginIndex(meta.id) != -1)
        return;

    const int insertIndex = getPluginInsertIndex(meta.id);
    m_pluginsList.insert(insertIndex, meta);

    emit pluginInserted(insertIndex, meta);
    qDebug() << "insert" << meta.id;
}

void PluginsManager::removePlugin(const ModuleMetaData &meta)
{
    if (pluginIndex(meta.id) == -1)
        return;

    for (auto it(m_pluginsList.begin()); it != m_pluginsList.end(); ++it)
    {
        if (it->id == meta.id)
        {
            m_pluginsList.erase(it);
            break;
        }
    }

    emit pluginRemoved(meta);
    qDebug() << "remove" << meta.id;
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
        if (checkDependentCondition(condition))
            insertPlugin(meta);
        else
            removePlugin(meta);
    }
}

DeviceMoniter::DeviceMoniter(QObject *parent)
    : QThread(parent)
{
    m_wacomInter = new DBusWacom("com.deepin.daemon.InputDevices", "/com/deepin/daemon/InputDevice/Wacom", QDBusConnection::sessionBus(), this);
    m_bluetoothInter = new DBusBluetooth("com.deepin.daemon.InputDevices", "/com/deepin/daemon/Bluetooth", QDBusConnection::sessionBus(), this);
}

void DeviceMoniter::run()
{
    while (true)
    {
        bool changed = false;

        const bool wacom = m_wacomInter->exist();
        if (wacom != m_wacomExist)
        {
            m_wacomExist = wacom;
            changed = true;
        }

        const int bluetoothState = m_bluetoothInter->state();
        const bool bluetooth = bluetoothState != 0;
        if (bluetooth != m_bluetoothExist)
        {
            m_bluetoothExist = bluetooth;
            changed = true;
        }

        if (changed)
            emit deviceChanged();

        QThread::sleep(1);
    }
}
