import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0

Flickable{

    contentWidth: width
    contentHeight: childrenRect.height

    Column {
        id:body
        width: networkModule.width

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
            sourceComponent: WirelessDevicesArea{}
        }

        Loader {
            id:vpn
            active: true
            sourceComponent: DBaseExpand{
                id:c3
                width: networkModule.width
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
}
