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
import DBus.Com.Deepin.Daemon.Bluetooth 1.0

Item {
    id:managerItem

    property string parentAppletPath

    readonly property string nmConnectionTypeVpn: "vpn"
    property var dbusNetwork: NetworkManager{}
    property var nmConnections: unmarshalJSON(dbusNetwork.connections)
    property var activeConnections: unmarshalJSON(dbusNetwork.activeConnections)
    property var nmDevices: JSON.parse(dbusNetwork.devices)
    property var wiredDevices:nmDevices["wired"] == undefined ? [] : nmDevices["wired"]
    property var vpnConnections: nmConnections[nmConnectionTypeVpn]
    property var vpnConnectionsCount:{
        if (vpnConnections)
            return vpnConnections.length
        else
            return 0
    }

    property var wirelessDevices: nmDevices["wireless"] == undefined ? [] : nmDevices["wireless"]
    property var wirelessDevicesCount:{
        if (wirelessDevices)
            return wirelessDevices.length
        else
            return 0
    }
    property int oldWirelessDevicesCount:0

    property var dbusBluetooth: Bluetooth{}
    property var bluetoothAdapters: unmarshalJSON(dbusBluetooth.adapters)
    property var blueToothAdaptersCount: {
        if (bluetoothAdapters)
            return bluetoothAdapters.length
        else
            return 0
    }
    property int oldBluetoothAdaptersCount:0

    property var wirelessListModel: ListModel {}
    property var bluetoothListModel: ListModel {}

    property var dockMode:dockDisplayMode
    onDockModeChanged: {
        print ("==> [Info] Dock display mode change...",dockMode)
        if (dockMode == 0)
            updateSettingItem(false)
        else
            updateSettingItem(true)
    }

    onParentAppletPathChanged: {
        updateWirelessApplet()
        updateBluetoothAdaperts()
    }

    onWirelessDevicesCountChanged: {
        //update wireless model
        if (getParentAppletPathHead() != "" && oldWirelessDevicesCount != wirelessDevicesCount){
            updateWirelessApplet()
            oldWirelessDevicesCount = wirelessDevicesCount
        }

        updateSettingItem(dockDisplayMode != 0)
    }

    onBlueToothAdaptersCountChanged: {
        //update bluetooth model
        if (getParentAppletPathHead() != "" && oldBluetoothAdaptersCount != blueToothAdaptersCount){
            updateBluetoothAdaperts()
            oldBluetoothAdaptersCount = blueToothAdaptersCount
        }

        updateSettingItem(dockDisplayMode != 0)
    }

    onVpnConnectionsCountChanged: {
        if (vpnConnectionsCount == 0)
            clearVPNAppletInfos()
        else
            insertVPNAppletInfos()
    }

    Timer {
        id:delayUpdateVPNInfosTimer
        repeat: false
        interval: 1000
        running: true
        onTriggered: {
            if (vpnConnectionsCount == 0)
                clearVPNAppletInfos()
            else
                insertVPNAppletInfos()
        }
    }

    function getParentAppletPathHead(){
        if (parentAppletPath != "")
            return parentAppletPath.substring(0,parentAppletPath.length - 8)//"main.qml" got 8 character
    }

    function updateWirelessApplet(){
        print("==> [Info] Updating Wifi applet....................")
        clearWirelessAppletInfos()
        wirelessListModel.clear()

        if (wirelessDevicesCount == 1){
            wirelessListModel.append({
                                         "applet_id": "wifi",
                                         "applet_path": getParentAppletPathHead() + "wifi/main.qml"
                                     })
        }
        else {
            for (var i = 0; i < wirelessDevicesCount; i ++){
                wirelessListModel.append({
                                             "applet_id": wirelessDevices[i].Vendor,
                                             "applet_path": getParentAppletPathHead() + "wifi/main.qml"
                                         })
            }
        }
    }

    function clearWirelessAppletInfos(){
        for (var i = 0; i < appletInfos.count; i ++){
            if (appletInfos.get(i).applet_icon.indexOf("wifi") > 0){
                appletInfos.remove(i)
                clearWirelessAppletInfos()
            }
        }
    }

    function clearVPNAppletInfos(){
        for (var i = 0; i < appletInfos.count; i ++){
            if (appletInfos.get(i).applet_icon.indexOf("vpn") > 0){
                appletInfos.remove(i)
            }
        }
    }

    function insertVPNAppletInfos() {
        for (var i = 0; i < appletInfos.count; i ++){
            if (appletInfos.get(i).applet_icon.indexOf("vpn") > 0){
                return
            }
        }

        //not exist ,insert new one
        appletInfos.append({
                   "applet_id": "vpn",
                   "applet_name": vpnLoader.item.name,
                   "applet_visible": vpnLoader.item.show,
                   "applet_icon":vpnLoader.item.iconPath,
                   "setting_enable":true
               })
    }

    function updateBluetoothAdaperts(){
        print ("==> [Info:] Updating Bluetooth applet...")
        clearBluetoothAppletInfos()
        bluetoothListModel.clear()


        //only support one device now
        if (blueToothAdaptersCount >= 1){
            bluetoothListModel.append({
                                          "applet_id": "bluetooth",
                                          "applet_path": getParentAppletPathHead() + "bluetooth/main.qml"
                                      })
        }

        //use the below code when support MultiDevice
//        if (blueToothAdaptersCount == 1){
//            bluetoothListModel.append({
//                                         "applet_id": "bluetooth",
//                                         "applet_path": getParentAppletPathHead() + "bluetooth/main.qml"
//                                     })
//        }
//        else {
//            for (var i = 0; i < blueToothAdaptersCount; i ++){
//                bluetoothListModel.append({
//                                             "applet_id": bluetoothAdapters[i].Vendor,
//                                             "applet_path": getParentAppletPathHead() + "bluetooth/main.qml"
//                                         })
//            }
//        }
    }

    function clearBluetoothAppletInfos() {
        for (var i = 0; i < appletInfos.count; i ++){
            if (appletInfos.get(i).applet_icon.indexOf("bluetooth") > 0){
                appletInfos.remove(i)
                clearBluetoothAppletInfos()
            }
        }
    }

    function updateAppletState(applet_id,new_state){
        print ("==> Update applet state...",applet_id,new_state)
        if (applet_id == "vpn")
            vpnLoader.item.setAppletState(new_state)
        else{
            //try wireless devices applet
            for (var i = 0; i < wirelessRepeater.count; i ++){
                print("wireless...:",wirelessRepeater.itemAt(i).item.appletId)
                if(wirelessListModel.get(i).applet_id == applet_id){
                    wirelessRepeater.itemAt(i).item.setAppletState(new_state)
                    return
                }
            }
            //try bluetooth device applet
            for (i = 0; i < bluetoothRepeater.count; i ++){
                if (bluetoothListModel.get(i).applet_id == applet_id){
                    bluetoothRepeater.itemAt(i).item.setAppletState(new_state)
                }
            }
        }
    }

    function updateSettingItem(showFlag){
        for (var i = 0; i < appletInfos.count; i ++){
            if (appletInfos.get(i).applet_icon.indexOf("vpn") > 0 ||
                    appletInfos.get(i).applet_icon.indexOf("wifi") > 0 ||
                    appletInfos.get(i).applet_icon.indexOf("bluetooth") > 0){
                appletInfos.get(i).setting_enable = showFlag
            }
        }
    }

    Timer {
        id:startupRefreshTImer
        repeat: false
        running: true
        interval: 1000
        onTriggered: updateSettingItem(dockDisplayMode != 0)
    }

    SubAppletItemLoader {
        id:vpnLoader
        appletId:"vpn"
        qmlPath:getParentAppletPathHead() + "vpn/main.qml"
    }

    Repeater {
        id: wirelessRepeater
        model: wirelessListModel
        delegate: SubAppletItemLoader {}
    }

    Repeater {
        id:bluetoothRepeater
        model:bluetoothListModel
        delegate: SubAppletItemLoader {}
    }

}
