import QtQuick 2.0
import Deepin.Widgets 1.0

Column {
    width: parent.width
    visible: vpnConnectionNumber > 0

    property var vpnConnections: nmConnections[nmConnectionTypeVpn]
    property int vpnConnectionNumber: vpnConnections ? vpnConnections.length : 0

    DBaseExpand {
        id: vpnExpand

        expanded: true
        header.sourceComponent: DBaseLine{
            id: wiredDeviceHeader
            leftLoader.sourceComponent: DssH2 {
                anchors.verticalCenter: parent.verticalCenter
                text: dsTr("VPN Connections")
            }

            rightLoader.sourceComponent: DSwitchButton{
                checked: vpnExpand.expanded
                onClicked: {
                    //dbusNetwork.vpnEnabled = checked
                    vpnExpand.expanded = !vpnExpand.expanded
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
