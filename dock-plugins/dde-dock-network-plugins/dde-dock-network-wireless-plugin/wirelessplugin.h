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
    bool configurable(const QString &id) Q_DECL_OVERRIDE;
    bool enabled(const QString &id) Q_DECL_OVERRIDE;
    void setEnabled(const QString &id, bool enabled) Q_DECL_OVERRIDE;
    void changeMode(Dock::DockMode newMode, Dock::DockMode oldMode) Q_DECL_OVERRIDE;
    void invokeMenuItem(QString, QString, bool) Q_DECL_OVERRIDE;

private:
    QSettings * m_settings;
    DockPluginProxyInterface * m_proxy;
    QMap<QString, WirelessItem *> m_itemMap;
    QStringList m_uuids;
    Dock::DockMode m_mode = Dock::EfficientMode;
    DBusNetwork * m_dbusNetwork;

    void initSettings();
    void updateUuids();
    void addNewItem(const QString &id);
    void removeItem(const QString &id);
    void onEnabledChanged(const QString &id);
    void onDevicesChanged();

    QString settingEnabledKey(const QString &id);
};

#endif // WIRELESSPLUGIN_H
