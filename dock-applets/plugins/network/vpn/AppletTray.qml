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


import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.DockApplet 1.0
import Deepin.AppletWidgets 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Network 1.0

DockApplet{
    id: vpnApplet
    title: "VPN"
    appid: "AppletVPN"
    icon: vpnEnable ? getIconUrl("vpn/vpn-enable.png") : getIconUrl("vpn/vpn-disable.png")

    property int xEdgePadding: 2
    property int titleSpacing: 10
    property int rootWidth: 180

    property var vpnEnable:dbusNetwork.vpnEnabled
    readonly property var nmActiveConnectionStateActivating: 1
    readonly property var nmActiveConnectionStateActivated: 2
    readonly property var nmConnectionTypeVpn: "vpn"
    property var nmActiveConnections: unmarshalJSON(dbusNetwork.activeConnections)
    property var nmConnections: unmarshalJSON(dbusNetwork.connections)
    property var vpnConnections: nmConnections[nmConnectionTypeVpn]
    property int vpnConnectionNumber: vpnConnections ? vpnConnections.length : 0

    function unmarshalJSON(valueJSON) {
        if (!valueJSON) {
            print("==> [ERROR] unmarshalJSON", valueJSON)
        }
        var value = JSON.parse(valueJSON)
        return value
    }


    function showNetwork(id){
        dbusControlCenter.ShowModule("network")
    }

    function hideVPN(id){
        setAppletState(false)
    }

    menu: AppletMenu{
        Component.onCompleted: {
            addItem(dsTr("_Run"), showNetwork);
            addItem(dsTr("_Undock"), hideVPN);
        }
    }

    onActivate:{
        showNetwork(0)
    }

    window: DockQuickWindow {
        id: root
        width: rootWidth
        height: content.height + xEdgePadding * 2
        color: "transparent"

        onNativeWindowDestroyed: {
            toggleAppletState("vpn")
            toggleAppletState("vpn")
        }

        onQt5ScreenDestroyed: {
            console.log("Recive onQt5ScreenDestroyed")
            mainObject.restartDockApplet()
        }

        Item {
            width: parent.width
            height: content.height
            anchors.centerIn: parent

            Column {
                id: content
                width: parent.width
                spacing: 10

                DBaseLine {
                    height: 30
                    width: parent.width
                    color: "transparent"
                    leftLoader.sourceComponent: DssH2 {
                        text: dsTr("VPN")
                        color: "#ffffff"
                    }

                    rightLoader.sourceComponent: DSwitchButton {
                        Connections {
                            // TODO still need connections block here, but why?
                            target: vpnApplet
                            onVpnEnableChanged: {
                                checked = vpnApplet.vpnEnable
                            }
                        }
                        checked: vpnApplet.vpnEnable
                        onClicked: dbusNetwork.vpnEnabled = checked
                    }
                }


                Rectangle {
                    width: rootWidth
                    height: vpnConnectlist.height
                    visible: vpnApplet.vpnEnable
                    color: "transparent"

                    ListView {
                        id: vpnConnectlist
                        width: parent.width
                        height: childrenRect.height
                        boundsBehavior: Flickable.StopAtBounds
                        model: vpnConnectionNumber
                        delegate: ConnectItem {}
                    }
                }
            }
        }
    }

}
