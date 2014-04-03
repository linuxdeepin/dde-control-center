import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    id: wirelessDevicesArea
    width: parent.width
    height: wirelessDevicesList.height + 2
    property int wirelessDevicesNumber: dbusNetwork.wirelessDevices.length
    property int wirelessAreaMaxHeight: 0

    ListView{
        id: wirelessDevicesList
        width: parent.width
        height: Math.min(childrenRect.height, parent.wirelessAreaMaxHeight - 2)
        model: wirelessDevicesNumber
        clip: true
        delegate: WirelessDeviceExpand {
            devicePath: dbusNetwork.wirelessDevices[index][0]
        }
        onModelChanged: {
            //print("Repeater changed", dbusNetwork.wirelessDevices)
        }
    }
    
    DSeparatorHorizontal {
        anchors.bottom: parent.bottom
    }
}
