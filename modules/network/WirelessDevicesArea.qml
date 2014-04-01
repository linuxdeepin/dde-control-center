import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    id: wirelessDevicesArea
    width: parent.width
    height: wirelessDevicesList.height + 2

    ListView{
        id: wirelessDevicesList
        width: parent.width
        height: childrenRect.height
        model: nm.wirelessDevices
        delegate: WirelessDeviceExpand{ dev: nm.wirelessDevices[index] }
        onModelChanged: {
            //print("Repeater changed", nm.wirelessDevices)
        }
    }

    Connections{
        target: nm
        onWirelessDevicesChanged:{
        }
    }
    
    DSeparatorHorizontal {
        anchors.bottom: parent.bottom
    }
}
