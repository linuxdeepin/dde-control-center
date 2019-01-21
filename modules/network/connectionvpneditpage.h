/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONNECTIONVPNEDITPAGE_H
#define CONNECTIONVPNEDITPAGE_H

#include "connectioneditpage.h"

#include <networkmanagerqt/accesspoint.h>

#define ServiceTypeL2TP "org.freedesktop.NetworkManager.l2tp"
#define ServiceTypePPTP "org.freedesktop.NetworkManager.pptp"
#define ServiceTypeVPNC "org.freedesktop.NetworkManager.vpnc"
#define ServiceTypeOpenVPN "org.freedesktop.NetworkManager.openvpn"
#define ServiceTypeStrongSwan "org.freedesktop.NetworkManager.strongswan"
#define ServiceTypeOpenConnect "org.freedesktop.NetworkManager.openconnect"
#define ServiceTypeSSTP "org.freedesktop.NetworkManager.sstp"

namespace dcc {
namespace network {

class ConnectionVpnEditPage : public ConnectionEditPage
{
    Q_OBJECT

public:
    enum VpnType {L2TP, PPTP, VPNC, OPENVPN, STRONGSWAN, OPENCONNECT, SSTP};

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
    void exportConnConfig();
    void processConfigCA(const QString &file);
};

} /* network */ 
} /* dcc */ 

#endif /* CONNECTIONVPNEDITPAGE_H */
