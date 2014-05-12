import QtQuick 2.0
import Deepin.Widgets 1.0

Column {
    width: parent.width
    property var vpnConnections: nmConnections[nmConnectionTypeVpn]
    property int vpnConnectionNumber: vpnConnections.length

    DBaseExpand {
        id: vpnExpand

        expanded: dbusNetwork.vPNEnabled
        header.sourceComponent: DBaseLine{
            id: wiredDeviceHeader
            leftLoader.sourceComponent: DssH2 {
                anchors.verticalCenter: parent.verticalCenter
                text: dsTr("VPN Connections")
            }

            rightLoader.sourceComponent: DSwitchButton{
                checked: vpnExpand.expanded
                onClicked: {
                    dbusNetwork.vPNEnabled = checked
                }
            }
        }
        content.sourceComponent: Item {
            width: parent.width
            height: childrenRect.height

            ListView {
                width: parent.width
                height: childrenRect.height
                boundsBehavior: Flickable.StopAtBounds
                model: vpnConnectionNumber
                delegate: VpnItem {}
            }
        }
    }

    DSeparatorHorizontal {}
}
