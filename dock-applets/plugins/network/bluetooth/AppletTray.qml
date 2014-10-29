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

DockApplet{
    id: bluetoothApplet
    title: "Bluetooth"
    appid: "AppletBluetooth"
    icon: dbus_bluetooth.powered ? getIconUrl("bluetooth/bluetooth-enable.png") : getIconUrl("bluetooth/bluetooth-disable.png")

    property int xEdgePadding: 2
    property int titleSpacing: 10
    property int rootWidth: 180

    property var adapters: unmarshalJSON(dbus_bluetooth.adapters)
    property var devices: unmarshalJSON(dbus_bluetooth.devices)
    property bool deviceConnected: dbus_bluetooth.powered
    property var connectedDevicePaths: {
        var paths = []
        for (var i = 0; i< devices.length; i++) {
            if (devices[i].Connected == true) {
                paths.push(devices[i].Path)
            }
        }
        return paths
    }


    // helper functions
    function marshalJSON(value) {
        var valueJSON = JSON.stringify(value);
        return valueJSON
    }
    function unmarshalJSON(valueJSON) {
        if (!valueJSON) {
            print("==> [ERROR] unmarshalJSON", valueJSON)
        }
        var value = JSON.parse(valueJSON)
        return value
    }

    function showBluetooth(id){
        dbusControlCenter.ShowModule("bluetooth")
    }

    function hideBluetooth(id){
        setAppletState(false)
    }

    menu: AppletMenu{
        Component.onCompleted: {
            addItem(dsTr("_Run"), showBluetooth);
            addItem(dsTr("_Undock"), hideBluetooth);
        }
    }

    onActivate:{
        showBluetooth(0)
    }

    window: DockQuickWindow {
        id: root
        width: rootWidth
        height: content.height + xEdgePadding * 2
        color: "transparent"

        onNativeWindowDestroyed: {
            toggleAppletState("bluetooth")
            toggleAppletState("bluetooth")
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
                        text: dsTr("Bluetooth")
                        color: "#ffffff"
                    }

                    rightLoader.sourceComponent: DSwitchButton {
                        Connections {
                            target: bluetoothApplet
                            onDeviceConnectedChanged: {
                                check = deviceConnected
                            }
                        }

                        checked: deviceConnected
                        onClicked: dbus_bluetooth.powered = checked
                    }
                }

                Rectangle {
                    width: rootWidth
                    height: nearby_devices_list.height
                    visible: dbus_bluetooth.powered
                    color: "transparent"

                    ListView{
                        id: nearby_devices_list
                        width: parent.width
                        height: childrenRect.height

                        model: ListModel {
                            id: deviceModel
                            Component.onCompleted: {
                                var devInfos = unmarshalJSON(dbus_bluetooth.GetDevices())
                                clear()
                                for(var i in devInfos){
                                    addOrUpdateDevice(devInfos[i])
                                }
                            }
                            function addOrUpdateDevice(devInfo) {
                                print("-> addOrUpdateDevice", marshalJSON(devInfo)) // TODO
                                if (isDeviceExists(devInfo)) {
                                    updateDevice(devInfo)
                                } else {
                                    addDevice(devInfo)
                                }
                            }
                            function addDevice(devInfo) {
                                var insertIndex = getInsertIndex(devInfo)
                                print("-> addDevice", insertIndex)
                                insert(insertIndex, {
                                    "devInfo": devInfo,
                                    "item_id": devInfo.Path,
                                    "item_name": devInfo.Alias,
                                })
                            }
                            function updateDevice(devInfo) {
                                var i = getDeviceIndex(devInfo)
                                get(i).devInfo = devInfo
                                sortModel()
                            }
                            function removeDevice(devInfo) {
                                if (isDeviceExists(devInfo)) {
                                    var i = getDeviceIndex(devInfo)
                                    remove(i, 1)
                                }
                            }
                            function isDeviceExists(devInfo) {
                                if (getDeviceIndex(devInfo) != -1) {
                                    return true
                                }
                                return false
                            }
                            function getDeviceIndex(devInfo) {
                                for (var i=0; i<count; i++) {
                                    if (get(i).devInfo.Path == devInfo.Path) {
                                        return i
                                    }
                                }
                                return -1
                            }
                            function getInsertIndex(devInfo) {
                                for (var i=0; i<count; i++) {
                                    if (devInfo.RSSI >= get(i).devInfo.RSSI) {
                                        return i
                                    }
                                }
                                return count
                            }
                            function sortModel() {
                                var n;
                                var i;
                                for (n=0; n<count; n++) {
                                    for (i=n+1; i<count; i++) {
                                        if (get(n).devInfo.RSSI+5 < get(i).devInfo.RSSI) {
                                            move(i, n, 1);
                                            n=0; // Repeat at start since I can't swap items i and n
                                        }
                                    }
                                }
                            }
                        }

                        delegate: DeviceItem {

                            onItemClicked: {
                                if (state)
                                {
                                    dbus_bluetooth.DisconnectDevice(id)
                                    console.log("Disconnect device, id:",id)
                                }
                                else
                                {
                                    dbus_bluetooth.ConnectDevice(id)
                                    console.log("Connect device, id:",id)
                                }
                            }

                            Connections {
                                target: bluetoothApplet
                                onDevicesChanged: {
                                    for (var i=0; i<bluetoothApplet.devices.length; i++) {
                                        if (devInfo.Path == bluetoothApplet.devices[i].Path) {
                                            // print("-> update device properties", bluetooth.devices[i].Alias) // TODO test
                                            itemName = bluetoothApplet.devices[i].Alias
                                            isConnected = bluetoothApplet.devices[i].Connected
                                            console.log(itemName,isConnected)
                                        }
                                    }
                                }
                            }
                        }

                        Connections {
                            target: dbus_bluetooth
                            onDeviceAdded: {
                                print("-> onDeviceAdded", arg0)
                                var devInfo = unmarshalJSON(arg0)
                                deviceModel.addOrUpdateDevice(devInfo)
                            }
                            onDeviceRemoved: {
                                print("-> onDeviceRemoved", arg0)
                                var devInfo = unmarshalJSON(arg0)
                                deviceModel.removeDevice(devInfo)
                            }
                        }
                    }
                }

            }
        }
    }

}
