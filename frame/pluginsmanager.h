#ifndef PLUGINSMANAGER_H
#define PLUGINSMANAGER_H

#include <QObject>
#include <QList>
#include <QThread>

#include "modulemetadata.h"
#include "dbus/dbuswacom.h"
#include "dbus/dbusbluetooth.h"

class DeviceMoniter;
class PluginsManager : public QObject
{
    Q_OBJECT
public:
    static PluginsManager *getInstance(QObject *parent = nullptr);

    int pluginIndex(const QString pluginId) const;
    int pluginIndex(const ModuleMetaData &plugin) const;
    const QString pluginPath(const QString &pluginId) const;
    const QList<ModuleMetaData>&& pluginsList() const;
    const ModuleMetaData pluginMetaData(const QString &pluginId) const;

signals:
    void pluginInserted(const int position, const ModuleMetaData &meta) const;
    void pluginRemoved(const ModuleMetaData &meta) const;

private:
    explicit PluginsManager(QObject *parent = 0);
    bool checkDependentCondition(const QString &condition);
    int getPluginInsertIndex(const QString &id);
    void insertPlugin(const ModuleMetaData &meta);
    void removePlugin(const ModuleMetaData &meta);
    void loadPlugins();

private:
    DeviceMoniter *m_deviceMoniter = nullptr;
    QList<ModuleMetaData> m_pluginsList;
};

class DeviceMoniter : public QThread
{
    Q_OBJECT
    Q_PROPERTY(bool bluetooth MEMBER m_bluetoothExist)
    Q_PROPERTY(bool wacom MEMBER m_wacomExist)

public:
    DeviceMoniter(QObject *parent = nullptr);

    void run() Q_DECL_OVERRIDE;

signals:
    void deviceChanged();

private:
    bool m_bluetoothExist = false;
    bool m_wacomExist = false;
};

#endif // PLUGINSMANAGER_H
