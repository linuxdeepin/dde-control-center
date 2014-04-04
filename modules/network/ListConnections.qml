import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0

Column {
    id:body

    Loader{
        id: wired
        active: true
        width: parent.width
        sourceComponent: WiredDeviceExpand {}
    }

    Loader{
        id: wireless
        active: true
        width: parent.width

        sourceComponent: WirelessDevicesArea{
            wirelessAreaMaxHeight: body.height - wired.height - vpn.height - 2
        }
    }

    Loader {
        id:vpn
        active: true
        sourceComponent: DBaseExpand{
            id:c3
            width: networkModule.width
            expanded: dbusNetwork.vPNEnabled
            header.sourceComponent: DSwitchButtonHeader {
                text: dsTr("VPN Connections")
                active: c3.expanded
                onActiveChanged: {
                    dbusNetwork.vPNEnabled = active
                }
            }
            content.sourceComponent: Column {
                Repeater {
                    model: dbusNetwork.vPNConnections
                    DLabel {
                        height: 30
                        verticalAlignment: Text.AlignVCenter
                        width:parent.width
                        text: dbusNetwork.vPNConnections[index][1] 
                        DSeparatorHorizontal{}
                    }
                }
            }
        }
    }

    DSeparatorHorizontal {}
}
