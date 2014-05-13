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
    property var connectedDevice: {
        for (var i = 0; i< devices.length; i++) {
            if (devices[i].Connected == true) {
                return devices[i]
            }
        }
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
    onDevicesChanged: {
        nearby_devices_list.model.clear()
        for(var i=0; i<devices.length; i++){
            nearby_devices_list.model.append({
                                           "item_id": devices[i].Path,
                                           "item_name": devices[i].Alias
                                       })
        }
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
    
    DssTitle {
        text: dsTr("Bluetooth")
        width: parent.width
    }

    DSeparatorHorizontal {}

    DBaseLine {
        height: 38
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("ON/OFF")
        }
        rightLoader.sourceComponent: DSwitchButton {
            checked: dbus_bluetooth.powered
            onClicked: dbus_bluetooth.powered == checked
        }
    }

    DSeparatorHorizontal {}

    DBaseLine {
        id: discoverable_line
        height: 38
        visible: dbus_bluetooth.powered
        property string timeHint: ""
        leftLoader.sourceComponent: TextWithHint {
            text: "ShowDevice"
            hint: discoverable_line.timeHint
            textColor: dconstants.fgColor
            hintColor: dconstants.fgDarkColor
        }
        rightLoader.sourceComponent: DRadioButton {
            buttonModel: [
                {"buttonId": "visible", "buttonLabel": dsTr("Visible")},
                {"buttonId": "invisible", "buttonLabel": dsTr("Invisible")},
            ]

            initializeIndex: dbus_bluetooth.discoverable ? 0 : 1

            onItemSelected: {
                if (idx == 0) {
                    dbus_bluetooth.discoverable = true
                    dbus_bluetooth.discoverableTimeout = 120
                    discoverable_timer.restart()
                }
            }
            
            Component.onCompleted: dbus_bluetooth.discoverable = false
        }
    }

    DSeparatorHorizontal { visible: dbus_bluetooth.powered }

    DBaseLine {
        height: 38
        visible: dbus_bluetooth.powered        
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("Device name")
        }
        rightLoader.sourceComponent: DTextInput {
            text: dbus_bluetooth.alias

            onTextChanged: {
                dbus_bluetooth.alias = text
            }
        }
    }

    DSeparatorHorizontal { visible: dbus_bluetooth.powered }

    DBaseExpand {
        visible: dbus_bluetooth.powered        
        header.sourceComponent: DBaseLine {
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Devices nearby")
            }
            rightLoader.sourceComponent: RefreshButton {
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

            model: ListModel {}

            delegate: SelectItem{
                showTip: false
                labelLeftMargin: 15
                totalItemNumber: bluetooth.devices.length
                selectItemId: bluetooth.connectedDevice.Path

                onSelectAction: {
                    dbus_bluetooth.ConnectDevice(itemId)
                }
            }
        }
    }

    DSeparatorHorizontal { visible: dbus_bluetooth.powered }

    /* DBaseExpand { */
    /*     id: recently_expand */

    /*     header.sourceComponent: DDownArrowHeader { */
    /*         text: dsTr("Recently connected devices") */
    /*         onActiveChanged: recently_expand.expanded = active */
    /*     } */
    /* } */

    /* DSeparatorHorizontal {} */
}
