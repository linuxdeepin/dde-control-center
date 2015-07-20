#ifndef DISKMOUNTPLUGIN_H
#define DISKMOUNTPLUGIN_H

#include <QMap>
#include <QLabel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QIcon>
#include <QDebug>
#include "mainitem.h"
#include "dock/dockconstants.h"
#include "dock/dockplugininterface.h"
#include "dock/dockpluginproxyinterface.h"
#include "dbus/dbusdiskmount.h"
#include "diskcontent.h"


class DiskMountPlugin : public QObject, public DockPluginInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.deepin.Dock.PluginInterface" FILE "dde-dock-diskmount-plugin.json")
#endif // QT_VERSION >= 0x050000
    Q_INTERFACES(DockPluginInterface)

public:
    DiskMountPlugin();
    ~DiskMountPlugin();

    void init(DockPluginProxyInterface *proxy) Q_DECL_OVERRIDE;

    QString name() Q_DECL_OVERRIDE;

    QStringList uuids() Q_DECL_OVERRIDE;
    QString getTitle(QString uuid) Q_DECL_OVERRIDE;
    QWidget * getItem(QString uuid) Q_DECL_OVERRIDE;
    QWidget * getApplet(QString uuid) Q_DECL_OVERRIDE;
    void changeMode(Dock::DockMode newMode, Dock::DockMode oldMode) Q_DECL_OVERRIDE;

    QString getMenuContent(QString uuid) Q_DECL_OVERRIDE;
    void invokeMenuItem(QString uuid, QString itemId, bool checked) Q_DECL_OVERRIDE;

signals:
    void dockModeChanged(Dock::DockMode mode);

private:
    QList<MainItem *> m_itemList;
    QString m_uuid = "disk_mount_plugin";
    DockPluginProxyInterface * m_proxy;

    Dock::DockMode m_mode = Dock::EfficientMode;
    DBusDiskMount *m_diskMount;

private:
    void mountableDeviceChanged();
    void setMode(Dock::DockMode mode);
    QJsonObject createMenuItem(QString itemId,
                               QString itemName,
                               bool checkable = false,
                               bool checked = false);
};

#endif // DISKMOUNTPLUGIN_H
