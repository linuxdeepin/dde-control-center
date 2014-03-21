import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0

Column {
    id:body
    width: root.width

    DBaseLine {
        leftLoader.sourceComponent: DssH1{
            text: dsTr("Wired Connections")
        }
    }
    DSeparatorHorizontal{}

    Item {
        id: wiredDevicesArea
        width: parent.width
        height: childrenRect.height

        ListView {
            width: parent.width
            height: childrenRect.height
            model: nm.wiredDevices
            delegate: WiredItem {}
        }
    }

    Item {
        id: wirelessDevicesArea
        width: parent.width
        height: wirelessDevicesList.height

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
    }

    Loader {
        id:vpn
        active:true
        sourceComponent: DBaseExpand{
            id:c3
            width:root.width
            expanded: nm.vPNEnabled
            header.sourceComponent: DSwitchButtonHeader {
                text: dsTr("VPN Connections")
                active: c3.expanded
                onActiveChanged: {
                    nm.vPNEnabled = active
                }
            }
            content.sourceComponent: Column {
                Repeater {
                    model: nm.vPNConnections
                    DLabel {
                        height: 30
                        verticalAlignment: Text.AlignVCenter
                        width:parent.width
                        text: nm.vPNConnections[index][1] 
                        DSeparatorHorizontal{}
                    }
                }
            }
        }
    }

    DSeparatorHorizontal {}
}
