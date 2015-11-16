#ifndef WIRELESSPLUGIN_H
#define WIRELESSPLUGIN_H

#include <QObject>
#include <QSettings>

#include <dde-dock/dockconstants.h>
#include <dde-dock/dockplugininterface.h>
#include <dde-dock/dockpluginproxyinterface.h>

#include "dbus/dbusnetwork.h"
#include "wirelessitem.h"

class QLabel;
class WirelessPlugin : public QObject, public DockPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.Dock.PluginInterface" FILE "dde-dock-network-wireless-plugin.json")
    Q_INTERFACES(DockPluginInterface)

public:
    WirelessPlugin();
    ~WirelessPlugin() Q_DECL_OVERRIDE;

    void init(DockPluginProxyInterface *proxy) Q_DECL_OVERRIDE;

    QString getPluginName() Q_DECL_OVERRIDE;

    QStringList ids() Q_DECL_OVERRIDE;
    QString getName(QString id) Q_DECL_OVERRIDE;
    QString getTitle(QString id) Q_DECL_OVERRIDE;
    QString getCommand(QString id) Q_DECL_OVERRIDE;
    QString getMenuContent(QString id) Q_DECL_OVERRIDE;
    QPixmap getIcon(QString) Q_DECL_OVERRIDE;
    QWidget * getItem(QString id) Q_DECL_OVERRIDE;
    QWidget * getApplet(QString id) Q_DECL_OVERRIDE;
    bool isDisabled(QString id) Q_DECL_OVERRIDE;
    bool canDisable(QString id) Q_DECL_OVERRIDE;
    void setDisabled(QString id, bool disabled) Q_DECL_OVERRIDE;
    void changeMode(Dock::DockMode newMode, Dock::DockMode oldMode) Q_DECL_OVERRIDE;
    void invokeMenuItem(QString id, QString itemId, bool checked) Q_DECL_OVERRIDE;

private:
    Dock::DockMode m_mode = Dock::FashionMode;
    DockPluginProxyInterface * m_proxy;
    com::deepin::daemon::DBusNetwork * m_dbusNetwork;
    QSettings * m_settings;
    QMap<QString, QString> m_wirelessMap;
    QMap<QString, WirelessItem *> m_itemMap;

    void initSettings();
    void onDisableChanged(const QString &id);
    void onDevicesChanged();

    QMap<QString, QString> wirelessDevices();
    QStringList wiredDevicePaths();
    QString settingDisabledKey(const QString &id);
};

#endif // WIRELESSPLUGIN_H
