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
    name: wirelessDevices.length > 1 ? appletId : dsTr("Wireless Network")
    iconPath: getIconUrl("wifi/ap-signal-100.png")

    property var dockMode: dockDisplayMode

    property var dbusNetwork: NetworkManager{}
    property var nmDevices: JSON.parse(dbusNetwork.devices)
    property var wirelessDevices: nmDevices["wireless"] == undefined ? [] : nmDevices["wireless"]
    property var deviceCount: {
        if (wirelessDevices)
            return wirelessDevices.length
        else
            return 0
    }

    appletTrayLoader: Loader {
        sourceComponent: AppletTray{}
        active:  appletItem.show && dockMode != 0
    }
}
