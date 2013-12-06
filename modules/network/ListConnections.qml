import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../widgets"

Column {
    id:body
    width: root.width

    DSepratorHorizontal{ visible: wired.active}
    Loader {
        id:wired
        active: nm.hasWired
        sourceComponent: DBaseExpand {
            id:c1
            width:root.width
            expanded: nm.wiredEnabled
            header: DSwitcherButtonHeader {
                text: dsTr("Wired Connections")
                checked: c1.expanded
                onCheckedChanged: {
                    nm.wiredEnabled = checked
                }
            }
            content: Column {
                Repeater {
                    model: nm.wiredConnections
                    DLabel {
                        height: 30
                        verticalAlignment: Text.AlignVCenter
                        width:parent.width
                        text: nm.wiredConnections[index][1] 
                        DSepratorHorizontal{}
                    }
                }
            }
        }
    }

    Loader {
        id:wireless
        active: nm.hasWireless
        sourceComponent: DBaseExpand{
            width:root.width
            expanded: nm.wirelessEnabled
            id: c2
            header: DSwitcherButtonHeader {
                text: dsTr("Wireless Acess Points")
                checked: c2.expanded
                onCheckedChanged: {
                    nm.wirelessEnabled = checked
                }
            }
            content: ListAccessPoints {width:root.width; accessPoints: nm.aPs}
        }
    }

    Loader {
        id:vpn
        active:true
        sourceComponent: DBaseExpand{
            id:c3
            width:root.width
            expanded: nm.vPNEnabled
            header: DSwitcherButtonHeader {
                text: dsTr("VPN Connections")
                checked: c3.expanded
                onCheckedChanged: {
                    nm.vPNEnabled = checked
                }
            }
            content: Column {
                Repeater {
                    model: nm.vPNConnections
                    DLabel {
                        height: 30
                        verticalAlignment: Text.AlignVCenter
                        width:parent.width
                        text: nm.vPNConnections[index][1] 
                        DSepratorHorizontal{}
                    }
                }
            }
        }
    }
}
