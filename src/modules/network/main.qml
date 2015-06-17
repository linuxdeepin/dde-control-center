import QtQuick 2.1
import QtQuick.Window 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import DBus.Com.Deepin.Daemon.Network 1.0
import Deepin.Widgets 1.0
import "../shared"

Item {
    id: networkModule
    anchors.fill: parent

    // colors
    property color normalColor: DConstants.fgColor
    property color normalBorderColor: DConstants.contentBgColor
    property color errorColor: "#F48914"

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

    signal needSecretsEmit(string path, string encryptionName, string accessPointName,bool autoconnect)
    signal toNetworkIndexPage

    property bool inPasswordInputting: false

    property bool inAllConnectionPage: stackView.depth == 1
    property var indexPage: ConnectionList {}

    property var stackViewPages: {
        "indexPage": Qt.resolvedUrl("ConnectionList.qml"),
        "infoPage": Qt.resolvedUrl("Info.qml"),
        "editPage": Qt.resolvedUrl("ConnectionEdit.qml"),
        "addPage": Qt.resolvedUrl("ConnectionAdd.qml"),
        "addDslPage": Qt.resolvedUrl("add/AddDsl.qml"),
        "addMobilePage": Qt.resolvedUrl("add/AddMobile.qml"),
        "addVpnPage": Qt.resolvedUrl("add/AddVpn.qml"),
        "addWirelessPage": Qt.resolvedUrl("wireless/AddWireless.qml"),
        "wifiHotspotPage": Qt.resolvedUrl("wireless/WifiHotspot.qml"),
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
        if (stackView.currentItem.objectName == "BaseEditPage") {
            stackView.currentItem.connectionSession.Close()
        }
    }

    function gotoAddConnection(page, type, devicePath) {
        stackView.push({
            "item": stackViewPages[page],
            "properties": {"connectionSession": createConnection(type, devicePath)},
            "destroyOnPop": true
        })
        stackView.currentItemId = page
    }
    function gotoEditConnection(page, uuid, devicePath) {
        stackView.push({
            "item": stackViewPages[page],
            "properties": { "connectionSession": editConnection(uuid, devicePath)},
            "destroyOnPop": true
        })
        stackView.currentItemId = page
    }

    function marshalJSON(value) {
        var valueJSON = JSON.stringify(value);
        return valueJSON
    }

    function unmarshalJSON(valueJSON) {
        if (!valueJSON) {
            print("==> [ERROR] unmarshalJSON", valueJSON)
        }
        var value = JSON.parse(valueJSON)
        return value
    }

    function getDeviceDisplayName(devType) {
        switch(devType) {
        case nmDeviceTypeUnknown:
            return dsTr("Unknown");
        case nmDeviceTypeEthernet:
            return dsTr("Wired");
        case nmDeviceTypeWifi:
            return dsTr("Wireless");
        case nmDeviceTypeUnused1:
            return dsTr("Unused1");
        case nmDeviceTypeUnused2:
            return dsTr("Unused2");
        case nmDeviceTypeBt:
            return dsTr("Bluetooth");
        case nmDeviceTypeOlpcMesh:
            return dsTr("OLPC mesh");
        case nmDeviceTypeWimax:
            return dsTr("WiMax");
        case nmDeviceTypeModem:
            return dsTr("Modem");
        case nmDeviceTypeInfiniband:
            return dsTr("InfiniBand");
        case nmDeviceTypeBond:
            return dsTr("Bond");
        case nmDeviceTypeVlan:
            return dsTr("VLAN");
        case nmDeviceTypeAdsl:
            return dsTr("ADSL");
        case nmDeviceTypeBridge:
            return dsTr("Bridge");
        }
        return dsTr("Unknown");
    }

    function getConnectionDisplayName(connType) {
        switch (connType) {
        case nmConnectionTypeWired:
            return dsTr("Ethernet");
        case nmConnectionTypeWireless:
            return dsTr("802.11 WiFi");
        case nmConnectionTypeWirelessAdhoc:
            return dsTr("802.11 WiFi AdHoc");
        case nmConnectionTypeWirelessHotspot:
            return dsTr("802.11 WiFi Hotspot");
        case nmConnectionTypePppoe:
            return dsTr("PPPoE");
        case nmConnectionTypeMobile:
            return dsTr("Mobile");
        case nmConnectionTypeMobileGsm:
            return dsTr("Mobile GSM");
        case nmConnectionTypeMobileCdma:
            return dsTr("Mobile CDMA");
        case nmConnectionTypeVpn:
            return dsTr("VPN");
        case nmConnectionTypeVpnL2tp:
            return dsTr("VPN L2TP");
        case nmConnectionTypeVpnPptp:
            return dsTr("VPN PPTP");
        case nmConnectionTypeVpnVpnc:
            return dsTr("VPN VPNC");
        case nmConnectionTypeVpnOpenvpn:
            return dsTr("VPN OpenVPN");
        case nmConnectionTypeVpnOpenconnect:
            return dsTr("VPN OpenConnect");
        }
    }

    function getActiveConnectionInfo(uuid) {
        for (var i in nmActiveConnections) {
            if(nmActiveConnections[i].Uuid == uuid){
                return nmActiveConnections[i]
            }
        }
        return null
    }

    function getDeviceActiveConnection(devicePath) {
        for (var i in nmActiveConnections) {
            if (getIndexFromArray(devicePath, nmActiveConnections[i].Devices) != -1) {
                return nmActiveConnections[i].Uuid
            }
        }
        return ""
    }
    function isActivedConnection(devicePath, uuid) {
        for (var i in nmActiveConnections) {
            if (nmActiveConnections[i].Uuid == uuid && getIndexFromArray(devicePath, nmActiveConnections[i].Devices) != -1 && nmActiveConnections[i].State == nmActiveConnectionStateActivated) {
                return true
            }
        }
        return false
    }
    function isActivatingConnection(devicePath, uuid) {
        for (var i in nmActiveConnections) {
            if (nmActiveConnections[i].Uuid == uuid && getIndexFromArray(devicePath, nmActiveConnections[i].Devices) != -1 && nmActiveConnections[i].State == nmActiveConnectionStateActivating) {
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

        rightLoader.sourceComponent: Item {
            height: header.height

            ItemTooltip {
                id:networkTooltip
                anchors.right: addButton.left
                anchors.rightMargin: 5
                anchors.verticalCenter: parent.verticalCenter
                width: 200
                delayShowInterval: 500
                textItem.font.pixelSize: 12
            }

            DssAddButton{
                id: addButton
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: infoButton.left
                anchors.rightMargin: 4
                visible: stackView.currentItemId == "indexPage"
                onClicked: {
                    stackView.push(stackViewPages["addPage"])
                    stackView.currentItemId = "addPage"

                    networkTooltip.hideTipImmediately()
                }
                onStateChanged: {
                    if (state == "hovered" && !addButton.pressed){
                        networkTooltip.tooltip = dsTr("Add Network Connection")
                        networkTooltip.showTip()
                    }
                    else
                        networkTooltip.hideTip()
                }

            }

            DImageButton {
                id:infoButton
                normal_image: "images/info_normal.png"
                hover_image: "images/info_hover.png"
                press_image: "images/info_press.png"
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 6
                visible: stackView.currentItemId == "indexPage"
                onClicked: {
                    stackView.push(stackViewPages["infoPage"])
                    stackView.currentItemId = "infoPage"

                    networkTooltip.hideTipImmediately()
                }
                onStateChanged: {
                    if (state == "hovered" && visible){
                        networkTooltip.tooltip = dsTr("View Detailed Information")
                        networkTooltip.showTip()
                    }
                    else
                        networkTooltip.hideTip()
                }
            }
        }
    }

    DSeparatorHorizontal{
        anchors.top: header.bottom
        anchors.topMargin: 1
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
