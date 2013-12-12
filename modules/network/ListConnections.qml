import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../widgets"

Column {
    id:body
    width: root.width

    DSeparatorHorizontal{ visible: wired.active}
    Loader {
        id:wired
        active: nm.hasWired
        sourceComponent: DBaseExpand {
            id:c1
            width:root.width
            expanded: nm.wiredEnabled
            Binding{
                target: nm
                property: "wiredEnabled"
                value:c1.expanded
            }
            header.sourceComponent: DSwitchButtonHeader {
                text: dsTr("Wired Connections")
                Binding{
                    target: c1;
                    property: "expanded";
                    value:parent.active
                }
            }
            content.sourceComponent: Column {
                Repeater {
                    model: nm.wiredConnections
                    DLabel {
                        height: 30
                        verticalAlignment: Text.AlignVCenter
                        width:parent.width
                        text: nm.wiredConnections[index][1] 
                        DSeparatorHorizontal{}
                    }
                }
            }
        }
    }

    Repeater {
        model: 1//nm.wirelessDevices
        delegate: DBaseExpand {
            id: c2
            width: root.width
            expanded: nm.wirelessEnabled
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
