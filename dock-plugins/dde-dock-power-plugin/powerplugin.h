#ifndef POWERPLUGIN_H
#define POWERPLUGIN_H

#include <QObject>

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

    virtual QString name() Q_DECL_OVERRIDE;

    virtual QStringList uuids() Q_DECL_OVERRIDE;
    virtual QString getTitle(QString uuid) Q_DECL_OVERRIDE;
    virtual QWidget * getItem(QString uuid) Q_DECL_OVERRIDE;
    virtual QWidget * getApplet(QString uuid) Q_DECL_OVERRIDE;
    virtual void changeMode(Dock::DockMode newMode, Dock::DockMode oldMode) Q_DECL_OVERRIDE;

    virtual QString getMenuContent(QString uuid) Q_DECL_OVERRIDE;
    virtual void invokeMenuItem(QString uuid, QString itemId, bool checked) Q_DECL_OVERRIDE;

private:
    QString m_uuid;
    QLabel * m_label;
    DockPluginProxyInterface * m_proxy;
    Dock::DockMode m_mode;

    com::deepin::daemon::Power * m_dbusPower;

    void setMode(Dock::DockMode mode);

    QString getBatteryIcon(int percentage, bool plugged, bool symbolic = false);

private slots:
    void updateIcon();
};

#endif // POWERPLUGIN_H
