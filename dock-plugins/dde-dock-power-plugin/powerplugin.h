#ifndef POWERPLUGIN_H
#define POWERPLUGIN_H

#include <QObject>
#include <QSettings>

#include <dock/dockconstants.h>
#include <dock/dockplugininterface.h>
#include <dock/dockpluginproxyinterface.h>

#include "dbus/dbuspower.h"

class QLabel;
class PowerPlugin : public QObject, public DockPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DockPluginInterface_iid FILE "dde-dock-power-plugin.json")
    Q_INTERFACES(DockPluginInterface)

public:
    PowerPlugin();
    ~PowerPlugin() Q_DECL_OVERRIDE;

    virtual void init(DockPluginProxyInterface *proxy) Q_DECL_OVERRIDE;

    virtual QString getPluginName() Q_DECL_OVERRIDE;

    virtual QStringList ids() Q_DECL_OVERRIDE;
    virtual QString getName(QString id) Q_DECL_OVERRIDE;
    virtual QString getTitle(QString id) Q_DECL_OVERRIDE;
    virtual QString getCommand(QString id) Q_DECL_OVERRIDE;
    virtual bool canDisable(QString id) Q_DECL_OVERRIDE;
    virtual bool isDisabled(QString id) Q_DECL_OVERRIDE;
    virtual void setDisabled(QString id, bool disabled) Q_DECL_OVERRIDE;
    virtual QWidget * getItem(QString id) Q_DECL_OVERRIDE;
    virtual QWidget * getApplet(QString id) Q_DECL_OVERRIDE;
    virtual void changeMode(Dock::DockMode newMode, Dock::DockMode oldMode) Q_DECL_OVERRIDE;

    virtual QString getMenuContent(QString id) Q_DECL_OVERRIDE;
    virtual void invokeMenuItem(QString id, QString itemId, bool checked) Q_DECL_OVERRIDE;

private:
    QString m_id;
    QLabel * m_label;
    Dock::DockMode m_mode;
    DockPluginProxyInterface * m_proxy;
    com::deepin::daemon::Power * m_dbusPower;

    QSettings * m_settings;

    void setMode(Dock::DockMode mode);

    void initSettings();
    QString getBatteryIcon(int percentage, bool plugged, bool symbolic = false);

private slots:
    void updateIcon();
};

#endif // POWERPLUGIN_H
