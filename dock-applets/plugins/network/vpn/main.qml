/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Wanqing Yang
**
**  Author:     Wanqing Yang <yangwanqing@linuxdeepin.com>
**  Maintainer: Wanqing Yang <yangwanqing@linuxdeepin.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

import QtQuick 2.1
import Deepin.AppletWidgets 1.0
import DBus.Com.Deepin.Daemon.Network 1.0

AppletPlugin {
    id: appletItem

    managed: true
    show: true
    name: dsTr("VPN")
    iconPath: getIconUrl("vpn/vpn-enable.png")

    property var dockMode: dockDisplayMode

    property var dbusNetwork: NetworkManager{}
    readonly property string nmConnectionTypeVpn: "vpn"
    property var nmActiveConnections: unmarshalJSON(dbusNetwork.activeConnections)
    property var nmConnections: unmarshalJSON(dbusNetwork.connections)
    property var vpnConnections: nmConnections[nmConnectionTypeVpn]
    property int vpnConnectionNumber: vpnConnections ? vpnConnections.length : 0

    appletTrayLoader: Loader {
        sourceComponent: AppletTray{}
        active:vpnConnectionNumber > 0 && appletItem.show && dockMode != 0//not mac mode
    }
}
