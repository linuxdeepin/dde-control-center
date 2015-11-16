#ifndef PLUGINSMANAGER_H
#define PLUGINSMANAGER_H

#include <QObject>
#include <QList>

#include "modulemetadata.h"

class PluginsManager : public QObject
{
    Q_OBJECT
public:
    static PluginsManager *getInstance(QObject *parent = nullptr);

    int pluginIndex(const QString pluginId) const;
    int pluginIndex(const ModuleMetaData &plugin) const;
    const QString pluginPath(const QString pluginId) const;
    const QList<ModuleMetaData>&& pluginsList() const;

signals:

private:
    explicit PluginsManager(QObject *parent = 0);
    void checkDevices();
    bool checkDeviceDependent(const QString &condition);
    int getPluginInsertIndex(const QString &id);
    void insertPlugin(const ModuleMetaData &meta);
    void loadPlugins();

private:
    QList<ModuleMetaData> m_pluginsList;

    bool m_bluetoothExist = false;
    bool m_wacomExist = false;
};

#endif // PLUGINSMANAGER_H
