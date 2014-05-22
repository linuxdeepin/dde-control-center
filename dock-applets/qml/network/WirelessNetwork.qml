import QtQuick 2.0
import Deepin.Widgets 1.0

Column {
    id: rootWirelessNetwork
    width: parent.width
    height: childrenRect.height
    property int deviceStatus: wirelessDevices[index].State
    property string devicePath: wirelessDevices[index].Path
    property string deviceHwAddress: wirelessDevices[index].HwAddress
    property string activeAp: wirelessDevices[index].ActiveAp

    DBaseExpand {
        width: parent.width
        height: childrenRect.height
        headerRect.color: "transparent"
        contentRect.color: "transparent"
        separator.visible: false

        expanded: dbusNetwork.wirelessEnabled

        //expanded: deviceStatus != 20 // TODO
        header.sourceComponent: Item {
            width: parent.width
            height: 30
            DssH2{
                text: wirelessDevicesNumber > 1 ? dsTr("Wireless Network %1").arg(index + 1) : dsTr("Wireless Network")
                anchors.left: parent.left
                anchors.leftMargin: 24
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

        content.sourceComponent: WirelessApList {}
    }
}
