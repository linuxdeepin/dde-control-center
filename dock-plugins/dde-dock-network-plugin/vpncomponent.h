#ifndef VPNCOMPONENT_H
#define VPNCOMPONENT_H

#include <QObject>
#include <QList>

#include "dbus/dbusnetwork.h"
#include "plugincomponentinterface.h"

struct VPN {
    QString id;
    QString uuid;
    bool connected;
};

class VPNApplet;
class QLabel;
class VPNComponent : public QObject, public PluginComponentInterface
{
    Q_OBJECT
public:
    friend class VPNApplet;

    VPNComponent(QString id, QObject *parent = 0);

    QString getName() Q_DECL_OVERRIDE;
    QString getTitle() Q_DECL_OVERRIDE;
    QString getCommand() Q_DECL_OVERRIDE;
    QWidget * getItem() Q_DECL_OVERRIDE;
    void retainItem() Q_DECL_OVERRIDE;
    QWidget * getApplet() Q_DECL_OVERRIDE;

    QString getMenuContent() Q_DECL_OVERRIDE;
    void invokeMenuItem(QString itemId, bool checked) Q_DECL_OVERRIDE;

signals:
    void vpnStatesChanged();

private:
    QLabel * m_item;
    VPNApplet * m_applet;
    QList<VPN> m_vpns;
    bool m_enabled;
    QString m_id;

    com::deepin::daemon::Network * m_dbusNetwork;

    void updateVPNs();
    bool isVPNConnected(QString path);
    bool isAnyVPNConnected();

private slots:
    void updateItem();
};

#endif // VPNCOMPONENT_H
