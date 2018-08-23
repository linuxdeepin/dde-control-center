#ifndef CONNECTIONVPNEDITPAGE_H
#define CONNECTIONVPNEDITPAGE_H

#include "connectioneditpagenew.h"

#include <networkmanagerqt/accesspoint.h>

#define ServiceTypeL2TP "org.freedesktop.NetworkManager.l2tp"
#define ServiceTypePPTP "org.freedesktop.NetworkManager.pptp"
#define ServiceTypeVPNC "org.freedesktop.NetworkManager.vpnc"
#define ServiceTypeOpenVPN "org.freedesktop.NetworkManager.openvpn"
#define ServiceTypeStrongSwan "org.freedesktop.NetworkManager.strongswan"
#define ServiceTypeOpenConnect "org.freedesktop.NetworkManager.openconnect"

namespace dcc {
namespace network {

class ConnectionVpnEditPage : public ConnectionEditPageNew
{
    Q_OBJECT

public:
    enum VpnType {L2TP, PPTP, VPNC, OPENVPN, STRONGSWAN, OPENCONNECT};

    explicit ConnectionVpnEditPage(const QString &connUuid = QString(), QWidget *parent = 0);
    virtual ~ConnectionVpnEditPage();

    // one of the following two methods must be called after initialization
    // init Widget using a exists vpn connection(specified by the "connUuid" of the constructor)
    void initSettingsWidget() Q_DECL_OVERRIDE;
    // init Widget by create a new vpn connection
    void initSettingsWidgetByType(ConnectionVpnEditPage::VpnType vpnType);

private:
    void initApSecretType(NetworkManager::AccessPoint::Ptr nmAp);
    void resetConnectionIdByType(ConnectionVpnEditPage::VpnType vpnType);
};

} /* network */ 
} /* dcc */ 

#endif /* CONNECTIONVPNEDITPAGE_H */
