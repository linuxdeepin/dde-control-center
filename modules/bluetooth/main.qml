import QtQuick 2.1
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Bluetooth 1.0

Column {
    id: bluetooth
    anchors.fill: parent

    property var dbusBluetooth: Bluetooth{}
    property var adapters: unmarshalJSON(dbusBluetooth.Adapters)
    property var devices: unmarshalJSON(dbusBluetooth.Devices)[dbusBluetooth.PrimaryAdapter]

    // TODO just for testing
    onAdaptersChanged: {
        for (var i in adapters) {
            print(adapters[i].Path, adapters[i].Alias, adapters[i].Powered)
        }
    }
    onDevicesChanged: {
        for (var i in devices) {
            print(devices[i].Path, devices[i].Alias)
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

        }
    }

    DSeparatorHorizontal {}

    DBaseLine {
        height: 38
        leftLoader.sourceComponent: TextWithHint {
            text: "ShowDevice"
            hint: "120s"
            textColor: dconstants.fgColor
            hintColor: dconstants.fgDarkColor
        }
        rightLoader.sourceComponent: DRadioButton {
            buttonModel: [
                {"buttonId": "visible", "buttonLabel": dsTr("Visible")},
                {"buttonId": "invisible", "buttonLabel": dsTr("Invisible")},
            ]

            initializeIndex: 0
        }
    }

    DSeparatorHorizontal {}

    DBaseLine {
        height: 38
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("Device name")
        }
        rightLoader.sourceComponent: DTextInput {
            text: "Toshiba"
        }
    }

    DSeparatorHorizontal {}

    DBaseExpand {
        header.sourceComponent: DBaseLine {
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Devices nearby")
            }
            rightLoader.sourceComponent: RefreshButton {
            }
        }
    }

    DSeparatorHorizontal {}

    DBaseExpand {
        id: recently_expand
        
        header.sourceComponent: DDownArrowHeader {
            text: dsTr("Recently connected devices")
            onActiveChanged: recently_expand.expanded = active
        }
    }

    DSeparatorHorizontal {}
}
