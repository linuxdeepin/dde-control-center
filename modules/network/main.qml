import QtQuick 2.1
import QtQuick.Window 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import DBus.Com.Deepin.Daemon.Network 1.0
import Deepin.Widgets 1.0
import DGui 1.0

Item {
    id: networkModule
    anchors.fill: parent

    // device type
    readonly property var nmDeviceTypeUnknown: "unknown"
    readonly property var nmDeviceTypeEthernet: "wired"
    readonly property var nmDeviceTypeWifi: "wireless"
    readonly property var nmDeviceTypeUnused1: "unused1"
    readonly property var nmDeviceTypeUnused2: "unused2"
    readonly property var nmDeviceTypeBt: "bt"
    readonly property var nmDeviceTypeOlpcMesh: "olpc-mesh"
    readonly property var nmDeviceTypeWimax: "wimax"
    readonly property var nmDeviceTypeModem: "modem"
    readonly property var nmDeviceTypeInfiniband: "infiniband"
    readonly property var nmDeviceTypeBond: "bond"
    readonly property var nmDeviceTypeVlan: "vlan"
    readonly property var nmDeviceTypeAdsl: "adsl"
    readonly property var nmDeviceTypeBridge: "bridge"

    // device state
    readonly property var nmDeviceStateUnknown: 0
    readonly property var nmDeviceStateUnmanaged: 10
    readonly property var nmDeviceStateUnavailable: 20
    readonly property var nmDeviceStateDisconnected: 30
    readonly property var nmDeviceStatePrepare: 40
    readonly property var nmDeviceStateConfig: 50
    readonly property var nmDeviceStateNeedAuth: 60
    readonly property var nmDeviceStateIpConfig: 70
    readonly property var nmDeviceStateIpCheck: 80
    readonly property var nmDeviceStateSecondaries: 90
    readonly property var nmDeviceStateActivated: 100
    readonly property var nmDeviceStateDeactivating: 110
    readonly property var nmDeviceStateFailed: 120

    // active connection state
    readonly property var nmActiveConnectionStateUnknown: 0
    readonly property var nmActiveConnectionStateActivating: 1
    readonly property var nmActiveConnectionStateActivated: 2
    readonly property var nmActiveConnectionStateDeactivating: 3
    readonly property var nmActiveConnectionStateDeactivate: 4

    // networking state
    readonly property var nmStateUnknown: 0
    readonly property var nmStateAsleep: 10
    readonly property var nmStateDisconnected: 20
    readonly property var nmStateDisconnecting: 30
    readonly property var nmStateConnecting: 40
    readonly property var nmStateConnectedLocal: 50
    readonly property var nmStateConnectedSite: 60
    readonly property var nmStateConnectedGlobal: 70
    
    // connection type
    readonly property var nmConnectionTypeWired: "wired"
    readonly property var nmConnectionTypeWireless: "wireless"
    readonly property var nmConnectionTypeWirelessAdhoc: "wireless-adhoc"
    readonly property var nmConnectionTypeWirelessHotspot: "wireless-hotspot"
    readonly property var nmConnectionTypePppoe: "pppoe"
    readonly property var nmConnectionTypeMobile: "mobile"
    readonly property var nmConnectionTypeMobileGsm: "mobile-gsm"
    readonly property var nmConnectionTypeMobileCdma: "mobile-cdma"
    readonly property var nmConnectionTypeVpn: "vpn"
    readonly property var nmConnectionTypeVpnL2tp: "vpn-l2tp"
    readonly property var nmConnectionTypeVpnPptp: "vpn-pptp"
    readonly property var nmConnectionTypeVpnVpnc: "vpn-vpnc"
    readonly property var nmConnectionTypeVpnOpenvpn: "vpn-openvpn"
    readonly property var nmConnectionTypeVpnOpenconnect: "vpn-openconnect"

    property var dbusNetwork: NetworkManager{}
    property var nmActiveConnections: unmarshalJSON(dbusNetwork.activeConnections)
    property var nmDevices: unmarshalJSON(dbusNetwork.devices)
    property var nmConnections: unmarshalJSON(dbusNetwork.connections)

    signal needSecretsEmit(string path, string encryptionName, string accessPointName)
    signal toNetworkIndexPage

    property bool inPasswordInputting: false

    property bool inAllConnectionPage: stackView.depth == 1
    property var indexPage: ConnectionList {}

    property var stackViewPages: {
        "indexPage": Qt.resolvedUrl("ConnectionList.qml"),
        "infoPage": Qt.resolvedUrl("Info.qml"),
        "editPage": Qt.resolvedUrl("ConnectionEdit.qml"),
        "indexAddPage": Qt.resolvedUrl("ConnectionAdd.qml"),
        "newDslPage": Qt.resolvedUrl("add/AddDsl.qml"),
        "newVpnPage": Qt.resolvedUrl("add/AddVpn.qml"),
        "hiddenAp": Qt.resolvedUrl("HiddenAp.qml"),
        "wifiHotspot": Qt.resolvedUrl("WifiHotspot.qml"),
    }

    Component {
        id: connectionSessionBuilder
        ConnectionSession {}
    }
    
    function createConnection(type, devicePath) {
        var connectionPath = dbusNetwork.CreateConnection(type, devicePath)
        return connectionSessionBuilder.createObject(null, { path: connectionPath })
    }
    
    function createConnectionForAccessPoint(apPath, devicePath) {
        var connectionPath = dbusNetwork.CreateConnectionForAccessPoint(apPath, devicePath)
        return connectionSessionBuilder.createObject(null, { path: connectionPath })
    }
    
    function editConnection(uuid, devicePath) {
        var connectionPath = dbusNetwork.EditConnection(uuid, devicePath)
        return connectionSessionBuilder.createObject(null, { path: connectionPath })
    }
    
    function resetConnectionSession() {
        if(stackView.currentItemId == "editPage" || stackView.currentItem == "hiddenAp"){
            stackView.currentItem.connectionSession.Close()
        }
    }

    function marshalJSON(value) {
        // TODO
        // if (value === undefined) {
        //     value = null
        // }
        var valueJSON = JSON.stringify(value);
        return valueJSON
    }
    
    function unmarshalJSON(valueJSON) {
        if (!valueJSON) {
            print("==> [ERROR] unmarshalJSON", valueJSON)
        }
        var value = JSON.parse(valueJSON)
        // TODO
        // if (value === null) {
        //     value = undefined
        // }
        return value
    }

    function getDeviceName(devType) {
        switch(devType) {
        case nmDeviceTypeUnknown:
            return dsTr("unknown");
        case nmDeviceTypeEthernet:
            return dsTr("wired");
        case nmDeviceTypeWifi:
            return dsTr("wireless");
        case nmDeviceTypeUnused1:
            return dsTr("unused1");
        case nmDeviceTypeUnused2:
            return dsTr("unused2");
        case nmDeviceTypeBt:
            return dsTr("bluetooth");
        case nmDeviceTypeOlpcMesh:
            return dsTr("olpc mesh");
        case nmDeviceTypeWimax:
            return dsTr("wimax");
        case nmDeviceTypeModem:
            return dsTr("modem");
        case nmDeviceTypeInfiniband:
            return dsTr("infiniband");
        case nmDeviceTypeBond:
            return dsTr("bond");
        case nmDeviceTypeVlan:
            return dsTr("vlan");
        case nmDeviceTypeAdsl:
            return dsTr("adsl");
        case nmDeviceTypeBridge:
            return dsTr("bridge");
        }
        return dsTr("unknown");
    }
    
    function getActiveConnectionInfo(uuid) {
        for (var i in nmActiveConnections) {
            if(nmActiveConnections[i].Uuid == uuid){
                return nmActiveConnections[i]
            }
        }
        return null
    }
    
    function isActiveConnection(devPath, uuid) {
        for (var i in nmActiveConnections) {
            if (getIndexFromArray(devPath, nmActiveConnections[i].Devices) != -1 &&
            nmActiveConnections[i].Uuid == uuid) {
                // TODO fix active state issue
            // nmActiveConnections[i].State == nmActiveConnectionStateActivated) {
                return true
            }
        }
        return false
    }
    
    Connections{
        target: rootWindow

        onModuleStartChange: {
            resetConnectionSession()
        }

        onPanelHided: {
            //resetConnectionSession()
            //stackView.reset()
        }
        onModuleIconClicked: {
            resetConnectionSession()
            stackView.reset()
        }
    }

    DssTitle {
        id: header
        text: modulesId.moduleLocaleNames["network"]

        onTitleClicked: {
            resetConnectionSession()
            stackView.reset()
        }

        rightLoader.sourceComponent: Row {
            height: header.height
            spacing: 4

            DssAddButton{
                id: addButton
                anchors.verticalCenter: parent.verticalCenter
                visible: stackView.currentItemId == "indexPage"
                onClicked: {
                    stackView.push(stackViewPages["indexAddPage"])
                    stackView.currentItemId = "indexAddPage"
                }
            }

            DTextButton {
                text: "i"
                anchors.verticalCenter: parent.verticalCenter
                visible: stackView.currentItemId == "indexPage"
                onClicked: {
                    stackView.push(stackViewPages["infoPage"])
                    stackView.currentItemId = "infoPage"
                }
            }
        }
    }

    DSeparatorHorizontal{
        anchors.top: header.bottom
        anchors.left: parent.left
        width: parent.width
    }

    DFlickable {
        id: flickableBox
        z: -1
        anchors.top: header.bottom
        anchors.topMargin: 2
        width: parent.width
        height: parent.height - header.height - 2

        contentWidth: parent.width
        contentHeight: stackView.height

        StackView {
            id:stackView
            width: parent.width
            height: currentItem.realHeight

            property string currentItemId: ""

            function reset(){
                stackView.pop(null)
                stackView.currentItemId = "indexPage"
            }

            Component.onCompleted: {
                stackView.push({
                    "item": stackViewPages["indexPage"],
                    "properties": { "width": parent.width }
                })
                stackView.currentItemId = "indexPage"
            }
        }
    }
}
