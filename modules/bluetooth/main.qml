import QtQuick 2.1
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Bluetooth 1.0
import "../shared/"

Column {
    id: bluetooth
    anchors.fill: parent

    Bluetooth { id: dbus_bluetooth }
    property var adapters: unmarshalJSON(dbus_bluetooth.adapters)
    property var devices: unmarshalJSON(dbus_bluetooth.devices)
    property var connectedDevicePaths: {
        var paths = []
        for (var i = 0; i< devices.length; i++) {
            if (devices[i].Connected == true) {
                paths.push(devices[i].Path)
            }
        }
        return paths
    }

    Timer {
        id: discoverable_timer
        repeat: true
        interval: 1000

        property int count: 0

        onTriggered: {
            if (count != 120) {
                count += 1
                discoverable_line.timeHint = (120 - count)
            } else {
                dbus_bluetooth.discoverable = false
                discoverable_timer.stop()
            }
        }
    }

    // TODO just for testing
    // onDevicesChanged: {
    //     nearby_devices_list.model.clear()
    //     for(var i=0; i<devices.length; i++){
    //         nearby_devices_list.model.append({
    //                                        "item_id": devices[i].Path,
    //                                        "item_name": devices[i].Alias
    //                                    })
    //     }
    // }

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
    
    DssTitle {
        text: modulesId.moduleLocaleNames["bluetooth"]
        width: parent.width
    }

    DSeparatorHorizontal {}

    DBaseLine {
        // TODO
        height: 38
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("ON/OFF")
        }
        rightLoader.sourceComponent: DSwitchButton {
            checked: dbus_bluetooth.powered
            onClicked: dbus_bluetooth.powered = checked
        }
    }

    DSeparatorHorizontal {}

    // TODO
    /* DBaseLine { */
    /*     id: discoverable_line */
    /*     height: 38 */
    /*     visible: dbus_bluetooth.powered */
    /*     property string timeHint: "" */
    /*     leftLoader.sourceComponent: TextWithHint { */
    /*         text: "ShowDevice" */
    /*         hint: discoverable_line.timeHint */
    /*         textColor: dconstants.fgColor */
    /*         hintColor: dconstants.fgDarkColor */
    /*     } */
    /*     rightLoader.sourceComponent: DRadioButton { */
    /*         buttonModel: [ */
    /*             {"buttonId": "visible", "buttonLabel": dsTr("Visible")}, */
    /*             {"buttonId": "invisible", "buttonLabel": dsTr("Invisible")}, */
    /*         ] */

    /*         initializeIndex: dbus_bluetooth.discoverable ? 0 : 1 */

    /*         onItemSelected: { */
    /*             if (idx == 0) { */
    /*                 dbus_bluetooth.discoverable = true */
    /*                 dbus_bluetooth.discoverableTimeout = 120 */
    /*                 discoverable_timer.restart() */
    /*             } */
    /*         } */
            
    /*         Component.onCompleted: dbus_bluetooth.discoverable = false */
    /*     } */
    /* } */

    /* DSeparatorHorizontal { visible: dbus_bluetooth.powered } */

    /* DBaseLine { */
    /*     height: 38 */
    /*     visible: dbus_bluetooth.powered         */
    /*     leftLoader.sourceComponent: DssH2 { */
    /*         text: dsTr("Device name") */
    /*     } */
    /*     rightLoader.sourceComponent: DTextInput { */
    /*         text: dbus_bluetooth.alias */

    /*         onTextChanged: { */
    /*             dbus_bluetooth.alias = text */
    /*         } */
    /*     } */
    /* } */

    /* DSeparatorHorizontal { visible: dbus_bluetooth.powered } */

    DBaseExpand {
        visible: dbus_bluetooth.powered
        header.sourceComponent: DBaseLine {
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Devices nearby")
            }
            rightLoader.sourceComponent: RefreshButton {
                // TODO
            }
        }
    }
    
    DSeparatorHorizontal { visible: dbus_bluetooth.powered }    

    Rectangle {
        width: parent.width
        height: nearby_devices_list.height
        visible: dbus_bluetooth.powered        
        color: dconstants.contentBgColor

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

            delegate: SelectItem{
                showTip: false
                labelLeftMargin: 15
                totalItemNumber: bluetooth.devices.length
                selected: {
                    if (devInfo) {
                        return getIndexFromArray(devInfo.Path, connectedDevicePaths) != -1
                    } else {
                        return false
                    }
                }
                onSelectAction: {
                    dbus_bluetooth.ConnectDevice(itemId)
                }
                Connections {
                    target: bluetooth
                    onDevicesChanged: {
                        for (var i=0; i<bluetooth.devices.length; i++) {
                            if (devInfo.Path == bluetooth.devices[i].Path) {
                                // print("-> update device properties", bluetooth.devices[i].Alias) // TODO test
                                itemName = bluetooth.devices[i].Alias
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

    DSeparatorHorizontal { visible: dbus_bluetooth.powered }
}
