import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../widgets"

Column {
    id:body
    width: root.width

    DSeparatorHorizontal{}
    DBaseHeader {
        title.sourceComponent: DssH1{
            text: dsTr("Wired Connections")
        }
    }
    DSeparatorHorizontal{}
    Repeater {
        model: nm.wiredDevices
        Column {
            width:root.width
            DBaseHeader {
                id:content
                color: dconstants.contentBgColor
                title.sourceComponent: DLabel {
                    height: 30
                    verticalAlignment: Text.AlignVCenter
                    width:parent.width
                    text: dsTr("Wired Conections") + index
                    color: nm.wiredDevices[index][1] != "/" ? "blue" : dconstants.fgColor
                    DSeparatorHorizontal{}
                }
                button.sourceComponent: DSwitchButton {
                    checked: nm.wiredDevices[index][1] != "/"
                }
            }
            DSeparatorHorizontal{}
        }
    }

    Repeater {
        model: 1//nm.wirelessDevices
        delegate: DBaseExpand {
            id: c2
            width: root.width
            expanded: true
            header.sourceComponent: DSwitchButtonHeader {
                text: dsTr("Wireless Access Points")
                active: c2.expanded
                onActiveChanged: {
                    nm.wirelessEnabled = active
                }
            }
            content.sourceComponent: ListAccessPoints {width:root.width; accessPoints: nm.aPs}
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
}
