import QtQuick 2.0
import Deepin.Widgets 1.0

Column {
    width: parent.width
    height: childrenRect.height

    DBaseExpand {
        width: parent.width
        height: childrenRect.height
        expanded: true

        headerRect.color: "transparent"
        contentRect.color: "transparent"

        separator.width: parent.width * 0.9

        header.sourceComponent: DSwitchButtonHeader{
            color: "transparent"
            text: {
                if (wirelessDeviceList.length > 1){
                    return dsTr("Wireless Network %1").arg(index + 1)
                }
                else{
                    return dsTr("Wireless Network")
                }
            }
        }

        content.sourceComponent: WirelessConnectionList {
            width: parent.width
            wirelessDevicePath: modelData
        }
    }

    DSeparatorHorizontal{}
}
