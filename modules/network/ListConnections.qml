import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../widgets"

Column {
    id:body
    width: root.width

    DSeparatorHorizontal{}
    DBaseLine {
        leftLoader.sourceComponent: DssH1{
            text: dsTr("Wired Connections")
        }
    }
    DSeparatorHorizontal{}
    Repeater {
        model: nm.wiredDevices
        Column {
            width:root.width
            DBaseLine {
                id:content
                color: dconstants.contentBgColor
                leftLoader.sourceComponent: DLabel {
                    height: 30
                    verticalAlignment: Text.AlignVCenter
                    width:parent.width
                    text: dsTr("Wired Conections") + ' "'+ nm.wiredDevices[index][1] + '"'
                    DSeparatorHorizontal{}
                }
                rightLoader.sourceComponent: DSwitchButton {
                    checked: nm.wiredDevices[index][1] == 100
                    onCheckedChanged: {
                        print("huhu", nm.wiredDevices[index][0], checked)
                        if (checked) {
                            nm.ActiveWiredDevice(checked, nm.wiredDevices[index][0])
                        } else {
                            nm.DisconnectDevice(nm.wiredDevices[index][0])
                        }
                    }
                }
            }
            DSeparatorHorizontal{}
        }
    }

    Repeater {
        model: nm.wirelessDevices
        delegate: ListAccessPoints{dev: nm.wirelessDevices[index]}
        onModelChanged: {
            print("Repeater changed", nm.wirelessDevices)
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
