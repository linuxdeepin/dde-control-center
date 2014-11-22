import QtQuick 2.1
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Bluetooth 1.0
import "../shared/"

Column {
    id: bluetooth
    anchors.fill: parent

    Bluetooth {
        id: dbus_bluetooth
        onAdapterAdded:updateAdapters()
        onAdapterRemoved:updateAdapters()
        onAdapterPropertiesChanged:{
            updateAdapters()
        }
    }
    property var adapters
    property int adaptersCount: {
        if (adapters)
            return adapters.length
        else
            return 0
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

    function updateAdapters(){
        adapters = unmarshalJSON(dbus_bluetooth.GetAdapters())
        if (adapters.length == 0)
            rootWindow.showModule("home")
    }

    Component.onCompleted: updateAdapters()

    DssTitle {
        text: modulesId.moduleLocaleNames["bluetooth"]
        width: parent.width
    }

    DSeparatorHorizontal {}



    ListView{
        id: bluetoothDevicesList
        width: parent.width
        height: childrenRect.height
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        model: adaptersCount
        delegate: BluetoothDeviceExpand {
            adapterIndex: index
            adapterCount: adaptersCount
            bluetoothAdapter: adapters[index]
        }
    }
}
