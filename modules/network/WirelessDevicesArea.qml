import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    id: wirelessDevicesArea
    width: parent.width
    height: wirelessDevicesList.height + 2
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
        height: childrenRect.height
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        model: wirelessDevicesNumber
        delegate: WirelessDeviceExpand {
            devicePath: wirelessDevices[index].Path
            deviceHwAddr: wirelessDevices[index].HwAddr
            activeAp: wirelessDevices[index].ActiveAp
            deviceStatus: wirelessDevices[index].State
        }
    }
    
    DSeparatorHorizontal {
        anchors.bottom: parent.bottom
        visible: wirelessDevicesNumber > 0
    }
}
