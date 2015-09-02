#ifndef POWERPLUGIN_H
#define POWERPLUGIN_H

#include <QObject>
#include <QSettings>

#include <dde-dock/dockconstants.h>
#include <dde-dock/dockplugininterface.h>
#include <dde-dock/dockpluginproxyinterface.h>

#include "dbus/dbuspower.h"

class QLabel;
class PowerPlugin : public QObject, public DockPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.Dock.PluginInterface" FILE "dde-dock-power-plugin.json")
    Q_INTERFACES(DockPluginInterface)

public:
    PowerPlugin();
    ~PowerPlugin() Q_DECL_OVERRIDE;

    void init(DockPluginProxyInterface *proxy) Q_DECL_OVERRIDE;

    QString getPluginName() Q_DECL_OVERRIDE;

    QStringList ids() Q_DECL_OVERRIDE;
    QString getName(QString id) Q_DECL_OVERRIDE;
    QString getTitle(QString id) Q_DECL_OVERRIDE;
    QString getCommand(QString id) Q_DECL_OVERRIDE;
    QPixmap getIcon(QString) Q_DECL_OVERRIDE;
    bool canDisable(QString id) Q_DECL_OVERRIDE;
    bool isDisabled(QString id) Q_DECL_OVERRIDE;
    void setDisabled(QString id, bool disabled) Q_DECL_OVERRIDE;
    QWidget * getItem(QString id) Q_DECL_OVERRIDE;
    QWidget * getApplet(QString id) Q_DECL_OVERRIDE;
    void changeMode(Dock::DockMode newMode, Dock::DockMode oldMode) Q_DECL_OVERRIDE;

    QString getMenuContent(QString id) Q_DECL_OVERRIDE;
    void invokeMenuItem(QString id, QString itemId, bool checked) Q_DECL_OVERRIDE;

private:
    QString m_id;
    QLabel * m_label;
    Dock::DockMode m_mode = Dock::FashionMode;
    DockPluginProxyInterface * m_proxy;
    com::deepin::daemon::DBusPower * m_dbusPower;

    QSettings * m_settings;

    void initSettings();
    void onDisableChanged();
    void setMode(Dock::DockMode mode);
    QString settingDisabledKey();
    QString getBatteryIcon(int percentage, bool plugged, bool symbolic = false);

private slots:
    void updateIcon();
};

#endif // POWERPLUGIN_H
