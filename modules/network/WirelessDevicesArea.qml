import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    id: wirelessDevicesArea
    width: parent.width
    height: wirelessDevicesList.height + 2
    // property int wirelessDevicesNumber: dbusNetwork.wirelessDevices.length // TODO remove
    property var wirelessDevices: nmDevices[nmDeviceTypeWifi]
    property int wirelessDevicesNumber: {
        if (wirelessDevices) {
            return wirelessDevices.length
        } else {
            return 0
        }
    }
    property int wirelessAreaMaxHeight: 0

    ListView{
        id: wirelessDevicesList
        width: parent.width
        height: Math.min(childrenRect.height, parent.wirelessAreaMaxHeight - 2)
        model: wirelessDevicesNumber
        clip: true
        delegate: WirelessDeviceExpand {
            // TODO fix refresh issue when remove a wireless device
            // devicePath: dbusNetwork.wirelessDevices[index][0] // TODO remove
            devicePath: wirelessDevices[index].Path
            deviceHwAddr: wirelessDevices[index].HwAddr
        }
        onModelChanged: {
            //print("Repeater changed", dbusNetwork.wirelessDevices)
        }
    }
    
    DSeparatorHorizontal {
        anchors.bottom: parent.bottom
        visible: wirelessDevicesNumber > 0
    }
}
