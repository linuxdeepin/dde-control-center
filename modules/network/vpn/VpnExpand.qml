import QtQuick 2.0
import Deepin.Widgets 1.0

Column {
    id: root
    width: parent.width
    visible: vpnConnectionNumber > 0

    property var vpnConnections: nmConnections[nmConnectionTypeVpn]
    property int vpnConnectionNumber: vpnConnections ? vpnConnections.length : 0
    property bool vpnEnabled: false
    
    Component.onCompleted: {
        vpnEnabled = isVpnEnabled()
    }
    
    function isVpnEnabled() {
        for (var i in nmActiveConnections) {
            if(nmActiveConnections[i].Vpn){
                return true
            }
        }
        return false
    }
    
    function getActiveVpnConnections() {
        var uuids = []
        for (var i in nmActiveConnections) {
            if(nmActiveConnections[i].Vpn){
                // only get activating or actived vpn connection
                if (nmActiveConnections[i].State == nmActiveConnectionStateActivating || nmActiveConnections[i].State == nmActiveConnectionStateActivated) {
                    uuids.push(nmActiveConnections[i].Uuid)
                }
            }
        }
        return uuids
    }
    
    DBaseExpand {
        id: vpnExpand

        expanded: vpnEnabled
        
        header.sourceComponent: DBaseLine{
            id: wiredDeviceHeader
            leftLoader.sourceComponent: DssH2 {
                anchors.verticalCenter: parent.verticalCenter
                text: dsTr("VPN Connections")
            }

            rightLoader.sourceComponent: DSwitchButton{
                property var lastActiveUuids
                checked: vpnExpand.expanded
                onClicked: {
                    if (checked) {
                        // enable vpn
                        vpnEnabled = true
                        for (var i in lastActiveUuids) {
                            dbusNetwork.ActivateConnection(lastActiveUuids[i], "/")
                        }
                    } else {
                        // disable vpn
                        vpnEnabled = false
                        lastActiveUuids = getActiveVpnConnections()
                        for (var i in lastActiveUuids) {
                            dbusNetwork.DeactivateConnection(lastActiveUuids[i])
                        }
                    }
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
