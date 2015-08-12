#ifndef PLUGINS_H
#define PLUGINS_H

#include <QMap>
#include <QObject>
#include <QString>
#include <QSettings>

#include <dock/dockconstants.h>
#include <dock/dockplugininterface.h>
#include <dock/dockpluginproxyinterface.h>

#include "plugincomponentinterface.h"
#include "bluetoothcomponent.h"
#include "vpncomponent.h"
#include "wiredcomponent.h"
#include "compositecomponent.h"

class QLabel;
class NetworkPlugin : public QObject, public DockPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DockPluginInterface_iid FILE "dde-dock-network-plugin.json")
    Q_INTERFACES(DockPluginInterface)

public:
    friend class VPNComponent;
    friend class WiredComponent;

    NetworkPlugin();
    ~NetworkPlugin() Q_DECL_OVERRIDE;

    void init(DockPluginProxyInterface *proxy) Q_DECL_OVERRIDE;

    QString getPluginName() Q_DECL_OVERRIDE;

    QStringList ids() Q_DECL_OVERRIDE;
    QString getName(QString id) Q_DECL_OVERRIDE;
    QString getTitle(QString id) Q_DECL_OVERRIDE;
    QString getCommand(QString id) Q_DECL_OVERRIDE;
    bool canDisable(QString id) Q_DECL_OVERRIDE;
    bool isDisabled(QString id) Q_DECL_OVERRIDE;
    void setDisabled(QString id, bool disabled) Q_DECL_OVERRIDE;
    QWidget * getItem(QString id) Q_DECL_OVERRIDE;
    QWidget * getApplet(QString id) Q_DECL_OVERRIDE;
    void changeMode(Dock::DockMode newMode, Dock::DockMode oldMode) Q_DECL_OVERRIDE;

    QString getMenuContent(QString id) Q_DECL_OVERRIDE;
    void invokeMenuItem(QString id, QString itemId, bool checked) Q_DECL_OVERRIDE;

private:
    DockPluginProxyInterface * m_proxy;
    QMap<QString, PluginComponentInterface*> m_items;

    Dock::DockMode m_mode;

    QSettings m_settings;

    void initSettings();
};

#endif // PLUGINS_H
