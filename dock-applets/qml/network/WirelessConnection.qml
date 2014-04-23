import QtQuick 2.0
import Deepin.Widgets 1.0

Column {
    width: parent.width
    height: childrenRect.height

    DBaseExpand {
        width: parent.width
        height: childrenRect.height
        expanded: dbusNetwork.wirelessEnabled

        headerRect.color: "transparent"
        contentRect.color: "transparent"
        separator.visible: false

        header.sourceComponent: Item {
            width: parent.width
            height: 30
            DssH2{
                text: wirelessDeviceList.length > 1 ? dsTr("Wireless Network %1").arg(index + 1) : dsTr("Wireless Network")
                anchors.left: parent.left
                anchors.leftMargin: 28
                anchors.verticalCenter: parent.verticalCenter
            }

            DSwitchButton {
                id: switchButton
                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                checked: dbusNetwork.wirelessEnabled
                onClicked: {
                    dbusNetwork.wirelessEnabled = checked
                }

                Connections {
                    target: dbusNetwork
                    onWirelessEnabledChanged:{
                        if (!switchButton.pressed){
                            switchButton.checked = dbusNetwork.wirelessEnabled
                        }
                    }
                }
            }
        }

        content.sourceComponent: WirelessConnectionList {
            width: parent.width
            wirelessDevicePath: modelData
        }
    }
}
