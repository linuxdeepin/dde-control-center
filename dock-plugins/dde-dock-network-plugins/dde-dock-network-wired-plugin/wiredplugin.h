#ifndef WIRELEDPLUGIN_H
#define WIRELEDPLUGIN_H

#include <QObject>
#include <QSettings>

#include <dde-dock/dockconstants.h>
#include <dde-dock/dockplugininterface.h>
#include <dde-dock/dockpluginproxyinterface.h>

#include "dbus/dbusnetwork.h"
#include "../network-data/networkdata.h"

class QLabel;
class WiredPlugin : public QObject, public DockPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.Dock.PluginInterface" FILE "dde-dock-network-wired-plugin.json")
    Q_INTERFACES(DockPluginInterface)

public:
    WiredPlugin();
    ~WiredPlugin() Q_DECL_OVERRIDE;

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
    bool configurable(const QString &id) Q_DECL_OVERRIDE;
    bool enabled(const QString &id) Q_DECL_OVERRIDE;
    void setEnabled(const QString &id, bool enabled) Q_DECL_OVERRIDE;
    void changeMode(Dock::DockMode newMode, Dock::DockMode oldMode) Q_DECL_OVERRIDE;
    void invokeMenuItem(QString, QString, bool) Q_DECL_OVERRIDE;

private:
    QLabel * m_wiredItem;
    QSettings * m_settings;
    DockPluginProxyInterface * m_proxy;
    Dock::DockMode m_mode = Dock::EfficientMode;
    com::deepin::daemon::DBusNetwork * m_dbusNetwork;

    void initSettings();
    void addNewItem(const QString &id);
    void removeItem(const QString &id);
    void onEnabledChanged(const QString &id);
    void onConnectionsChanged();
    QString getWiredIp();
    bool isWiredConnected();
    int wirelessDevicesCount();
    int wiredDevicesCount();

    QString settingEnabledKey(const QString &id);
};

#endif // WIRELEDPLUGIN_H
