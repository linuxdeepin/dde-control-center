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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import Deepin.DockApplet 1.0
import Deepin.AppletWidgets 1.0

Item {
    id: vpnItem
    width: parent.width
    height: 30

    Behavior on height {
        PropertyAnimation { duration: 100 }
    }

    property int connectionStatus: 100

    property var infos: vpnConnections[index]
    property var activeConnectionInfo: getActiveConnectionInfo(infos.Uuid)

    property bool isConnected: activeConnectionInfo && activeConnectionInfo.Vpn && activeConnectionInfo.State == nmActiveConnectionStateActivated

    function getActiveConnectionInfo(uuid) {
        for (var i in nmActiveConnections) {
            if(nmActiveConnections[i].Uuid == uuid){
                return nmActiveConnections[i]
            }
        }
        return null
    }

    function getActiveVpnConnections() {
        var uuids = []
        for (var i in nmActiveConnections) {
            if(nmActiveConnections[i].Vpn){
                // only get activating or actived vpn connection
                if (nmActiveConnections[i].State == nmActiveConnectionStateActivating || nmActiveConnections[i].State == nmActiveConnectionStateActivated) {
                    uuids.push(nmActiveConnections[i].Uuid)
                }
            }
        }
        return uuids
    }

    DImageButton {
        id: checkImg
        width: 14
        height: 14
        anchors {verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 10}
        normal_image: getIconUrl("bluetooth/connected.png")
        hover_image: getIconUrl("bluetooth/disconnect.png")
        visible: isConnected
        onClicked: {
            dbusNetwork.DeactivateConnection(infos.Uuid)
        }
    }

    AppletWaitingImage {
        anchors.left: checkImg.left
        anchors.verticalCenter: checkImg.verticalCenter
        on: activeConnectionInfo && activeConnectionInfo.Uuid == infos.Uuid && activeConnectionInfo.Vpn && activeConnectionInfo.State == nmActiveConnectionStateActivating
    }

    DssH2 {
        id:deviceText
        text: infos.Id
        width: text.length
        height: parent.height
        color: "#bcbcbc"

        verticalAlignment: Text.AlignVCenter
        anchors {left: checkImg.right; leftMargin: 5}
    }

    MouseArea {
        anchors.fill: parent
        anchors.leftMargin: checkImg.width + 15
        hoverEnabled: true
        onEntered: {
            deviceText.color = "#ffffff"
        }
        onExited: deviceText.color = "#bcbcbc"

        onClicked: {
            if (isConnected)
            {
                console.log("DisConnect VPN .........")
                dbusNetwork.DeactivateConnection(infos.Uuid)
            }

            else
            {
                console.log("Connect VPN ............")
                dbusNetwork.ActivateConnection(infos.Uuid, "/")
            }
        }
    }

}
