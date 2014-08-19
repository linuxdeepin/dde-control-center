/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Kaisheng Ye
**
**  Author:     Kaisheng Ye <kaisheng.ye@gmail.com>
**  Maintainer: Kaisheng Ye <kaisheng.ye@gmail.com>
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
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Network 1.0
import DBus.Com.Deepin.Daemon.Bluetooth 1.0
import DBus.Com.Deepin.Api.Graphic 1.0
import "../widgets/"

DockApplet{
    title: "Network"
    appid: "AppletNetwork"
    icon: dockDisplayMode == 0 ? macIconUri : winIconUri

    property var dconstants: DConstants {}
    property var activeConnections: unmarshalJSON(dbusNetwork.activeConnections)

    property string macIconUri: getIcon()
    property string winIconUri: getIconUrl("network/small/wired_on.png")

    // Graphic
    property var dbusGraphic: Graphic {}
    property string iconBgDataUri: {
        if(dbusNetwork.state == 70){
            var path = "network/normal/network_on.png"
        }
        else{
            var path = "network/normal/network_off.png"
        }
        return getIconDataUri(path)
    }
    property var subImageList: ListModel{
        function getTypeIndex(type){
            for(var i=0; i<subImageList.count; i++){
                var imageInfo = subImageList.get(i)
                if(imageInfo.type == type){
                    return i
                }
            }
            return -1

        }
    }

    function getIcon(){
        if(airplaneModeButton.airplaneModeActive){
            var iconDataUri = getIconDataUri("network/normal/airplane_mode.png")
            winIconUri = getIconUrl("network/small/airplane.png")
        }
        else{
            var iconDataUri = iconBgDataUri
            for(var i=0; i<subImageList.count; i++){
                var imageInfo = subImageList.get(i)
                iconDataUri = dbusGraphic.CompositeImageUri(
                    iconDataUri,
                    getIconDataUri(imageInfo.imagePath),
                    imageInfo.x,
                    imageInfo.y,
                    "png"
                )
            }

            if(activeWiredDevice){
                winIconUri = getIconUrl("network/small/wired_on.png")
            }
            else{
                if(activeWirelessDevice) {
                    winIconUri = getIconUrl(wifiStateDict.imagePath)
                }
                else{
                    winIconUri = getIconUrl("network/small/wired_off.png")
                }
            }
        }
        print("==> [info] network icon update...")
        return iconDataUri
    }

    function getIconDataUri(path){
        return dbusGraphic.ConvertImageToDataUri(getIconUrl(path).split("://")[1])
    }

    property var positions: {
        "vpn": [6, 6],
        "bluetooth": [6, 25],
        "3g": [25, 6],
        "wifi": [25, 25]
    }

    function updateState(type, show, imagePath){
        var index = subImageList.getTypeIndex(type)
        if(show){
            if(index == -1){
                subImageList.append({
                    "type": type,
                    "imagePath": imagePath,
                    "x": positions[type][0],
                    "y": positions[type][1]
                })
            }
            else{
                var info = subImageList.get(index)
                if(info.imagePath != imagePath){
                    info.imagePath = imagePath
                }
            }
        }
        else{
            if(index != -1){
                subImageList.remove(index)
            }
        }
    }

    // wired
    property var nmConnections: unmarshalJSON(dbusNetwork.connections)
    property var activeWiredDevice: getActiveWiredDevice()
    property bool hasWiredDevices: {
        if(nmDevices["wired"] && nmDevices["wired"].length > 0){
            return true
        }
        else{
            return false
        }
    }

    // wifi
    property bool hasWirelessDevices: {
        if(nmDevices["wireless"] && nmDevices["wireless"].length > 0){
            return true
        }
        else{
            return false
        }
    }
    property var wifiStateDict: { "show": true, "imagePath": "" }
    property var activeWirelessDevice: getActiveWirelessDevice()
    onActiveWirelessDeviceChanged: {
        if(activeWirelessDevice){
            var allAp = JSON.parse(dbusNetwork.GetAccessPoints(activeWirelessDevice.Path))
            for(var i in allAp){
                var apInfo = allAp[i]
                if(apInfo.Path == activeWirelessDevice.ActiveAp){
                    updateWifiState(true, apInfo)
                }
            }
        }
        else{
            if(hasWirelessDevices){
                updateWifiState(true, null)
            }
            else {
                updateWifiState(false, null)
            }
        }
    }

    Connections{
        target: dbusNetwork
        onAccessPointPropertiesChanged: {
            if(activeWirelessDevice && arg0 == activeWirelessDevice.Path){
                var apInfo = unmarshalJSON(arg1)
                if(apInfo.Path == activeWirelessDevice.ActiveAp){
                    updateWifiState(true, apInfo)
                }
            }
        }
    }

    function updateWifiState(show, apInfo){
        var image_id = 0
        if(show){
            if(!apInfo){
                image_id = 0
            }
            else{
                if(apInfo.Strength <= 25){
                    image_id = 25
                }
                else if(apInfo.Strength <= 50){
                    image_id = 50
                }
                else if(apInfo.Strength <= 75){
                    image_id = 75
                }
                else if(apInfo.Strength <= 100){
                    image_id = 100
                }
            }
        }
        wifiStateDict.show = show
        wifiStateDict.imagePath = "network/small/wifi_%1.png".arg(image_id)
        var imagePath = "network/normal/wifi_%1.png".arg(image_id)
        updateState("wifi", show, imagePath)
    }

    // vpn
    property var vpnConnections: nmConnections["vpn"]
    property int activeVpnIndex: {
        for(var i in activeConnections){
            if(activeConnections[i].Vpn){
                return i
            }
        }
        return -1
    }

    onActiveVpnIndexChanged: {
        var vpnShow = vpnConnections ? vpnConnections.length > 0 : false
        var vpnEnabled = activeVpnIndex != -1
        var imagePath = "network/normal/vpn_"
        if(vpnEnabled){
            imagePath += "on.png"
        }
        else{
            imagePath += "off.png"
        }
        updateState("vpn", vpnShow, imagePath)
    }
    onVpnConnectionsChanged: {
        var vpnShow = vpnConnections ? vpnConnections.length > 0 : false
        var vpnEnabled = activeVpnIndex != -1
        var imagePath = "network/normal/vpn_"
        if(vpnEnabled){
            imagePath += "on.png"
        }
        else{
            imagePath += "off.png"
        }
        updateState("vpn", vpnShow, imagePath)
    }

    // bluetooth
    property var dbusBluetooth: Bluetooth {}
    property var adapters: dbusBluetooth.adapters ? unmarshalJSON(dbusBluetooth.adapters) : ""

    onAdaptersChanged: {
        var show = adapters.length > 0
        var enabled = dbusBluetooth.powered ? dbusBluetooth.powered : false
        var imagePath = "network/normal/bluetooth_"
        if(enabled){
            imagePath += "on.png"
        }
        else{
            imagePath += "off.png"
        }
        updateState("bluetooth", show, imagePath)
    }


    property int xEdgePadding: 10

    function getActiveWirelessDevice(){
        for(var i in wirelessDevices){
            var info = wirelessDevices[i]
            if(info.ActiveAp != "/" && info.State == 100){
                return info
            }
        }
        return null
    }
    function getActiveWiredDevice(){
        for(var i in wiredDevices){
            var info = wiredDevices[i]
            if(info.State == 100){
                return info
            }
        }
        return null
    }

    function showNetwork(id){
        dbusControlCenter.ShowModule("network")
    }

    function hideNetwork(id){
        set_hide_applet("network")
    }

    onActivate: {
        showNetwork(0)
    }

    onNativeWindowDestroyed: {
        toggle_applet("network")
        toggle_applet("network")
    }

    onQt5ScreenDestroyed: {
        console.log("Recive onQt5ScreenDestroyed")
        mainObject.restartDockApplet()
    }

    menu: Menu {
        Component.onCompleted: {
            addItem(dsTr("_Run"), showNetwork);
            addItem(dsTr("_Undock"), hideNetwork);
        }
    }

    window: DockQuickWindow {
        id: root
        width: 224
        height: contentColumn.height + xEdgePadding * 2
        color: "transparent"

        Item {
            anchors.centerIn: parent
            width: parent.width - xEdgePadding * 2
            height: parent.height - xEdgePadding * 2

            Column {
                id: contentColumn
                width: parent.width
                spacing: 20

                Row {
                    id: buttonRow
                    spacing: 16
                    anchors.horizontalCenter: parent.horizontalCenter

                    CheckButton{
                        id: wiredCheckButton
                        onImage: "images/wire_on.png"
                        offImage: "images/wire_off.png"
                        visible: hasWiredDevices

                        onClicked: {
                            dbusNetwork.wiredEnabled = active
                        }

                        Connections{
                            target: dbusNetwork
                            onWiredEnabledChanged:{
                                if(!wiredCheckButton.pressed){
                                    wiredCheckButton.active = dbusNetwork.wiredEnabled
                                }
                            }
                        }

                        Timer{
                            running: true
                            interval: 100
                            onTriggered: {
                                parent.active = dbusNetwork.wiredEnabled
                            }
                        }
                    }

                    CheckButton{
                        id: wirelessCheckButton
                        onImage: "images/wifi_on.png"
                        offImage: "images/wifi_off.png"
                        visible: hasWirelessDevices

                        onClicked: {
                            dbusNetwork.wirelessEnabled = active
                        }

                        Connections{
                            target: dbusNetwork
                            onWirelessEnabledChanged:{
                                if(!wirelessCheckButton.pressed){
                                    wirelessCheckButton.active = dbusNetwork.wirelessEnabled
                                }
                            }
                        }

                        Timer{
                            running: true
                            interval: 100
                            onTriggered: {
                                parent.active = dbusNetwork.wirelessEnabled
                            }
                        }
                    }

                    //CheckButton{
                        //onImage: "images/3g_on.png"
                        //offImage: "images/3g_off.png"
                    //}

                    // TODO
                    CheckButton{
                        id: vpnButton
                        visible: vpnConnections ? vpnConnections.length > 0 : false
                        onImage: "images/vpn_on.png"
                        offImage: "images/vpn_off.png"
                        property bool vpnActive: activeVpnIndex != -1

                        // onVpnActiveChanged: {
                        //     if(!vpnButton.pressed){
                        //         vpnButton.active = vpnActive
                        //     }
                        // }

                        // function deactiveVpn(){
                        //     if(activeVpnIndex != -1){
                        //         var uuid = activeConnections[activeVpnIndex].Uuid
                        //         dbusNetwork.DeactivateConnection(uuid)
                        //     }
                        // }

                        // onClicked: {
                        //     deactiveVpn()
                        // }

                        onClicked: {
                            dbusNetwork.vpnEnabled = active
                        }

                        Connections{
                            target: dbusNetwork
                            onVpnEnabledChanged:{
                                if(!vpnButton.pressed){
                                    vpnButton.active = dbusNetwork.vpnEnabled
                                }
                            }
                        }
                        
                        Timer{
                            running: true
                            interval: 100
                            onTriggered: {
                                // parent.active = parent.vpnActive
                                parent.active = dbusNetwork.vpnEnabled
                            }
                        }
                    }

                    CheckButton{
                        id: bluetoothButton
                        visible: adapters.length > 0
                        onImage: "images/bluetooth_on.png"
                        offImage: "images/bluetooth_off.png"

                        onClicked: {
                            dbusBluetooth.powered = active
                        }

                        Connections{
                            target: dbusBluetooth
                            onPoweredChanged:{
                                if(!bluetoothButton.pressed && typeof(dbusBluetooth.powered) != "undefined"){
                                    bluetoothButton.active = dbusBluetooth.powered
                                }
                                var show = adapters.length > 0
                                var enabled = dbusBluetooth.powered
                                var imagePath = "network/normal/bluetooth_"
                                if(enabled){
                                    imagePath += "on.png"
                                }
                                else{
                                    imagePath += "off.png"
                                }
                                updateState("bluetooth", show, imagePath)
                            }
                        }

                        Timer{
                            running: true
                            interval: 100
                            onTriggered: {
                                if(dbusBluetooth.powered)
                                    parent.active = dbusBluetooth.powered
                            }
                        }
                    }

                    CheckButton {
                        id: airplaneModeButton
                        onImage: "images/airplane_mode_on.png"
                        offImage: "images/airplane_mode_off.png"
                        property bool airplaneModeActive: {
                            if(dbusNetwork.networkingEnabled || dbusBluetooth.powered){
                                return false
                            }
                            else{
                                return true
                            }
                        }

                        onAirplaneModeActiveChanged: {
                            if(!airplaneModeButton.pressed){
                                airplaneModeButton.active = airplaneModeButton.airplaneModeActive
                            }
                        }

                        function setActive(){
                            dbusNetwork.networkingEnabled = true
                            dbusBluetooth.powered = true
                        }

                        function setDeactive(){
                            dbusNetwork.networkingEnabled = false
                            dbusBluetooth.powered = false
                        }

                        onClicked: {
                            if(active){
                                setDeactive()
                            }
                            else{
                                setActive()
                            }
                        }

                        Timer{
                            running: true
                            interval: 100
                            onTriggered: {
                                parent.active = parent.airplaneModeActive
                            }
                        }

                    }
                }

            }

        }

    }

}
