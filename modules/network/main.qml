import QtQuick 2.1
import QtQuick.Window 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import DBus.Com.Deepin.Daemon.Network 1.0
import Deepin.Widgets 1.0
import DGui 1.0

Column{
    id: networkModule
    anchors.fill: parent

    // device type
    readonly property var nmDeviceTypeUnknown: 0
    readonly property var nmDeviceTypeEthernet: 1
    readonly property var nmDeviceTypeWifi: 2
    readonly property var nmDeviceTypeUnused1: 3
    readonly property var nmDeviceTypeUnused2: 4
    readonly property var nmDeviceTypeBt: 5
    readonly property var nmDeviceTypeOlpcMesh: 6
    readonly property var nmDeviceTypeWimax: 7
    readonly property var nmDeviceTypeModem: 8
    readonly property var nmDeviceTypeInfiniband: 9
    readonly property var nmDeviceTypeBond: 10
    readonly property var nmDeviceTypeVlan: 11
    readonly property var nmDeviceTypeAdsl: 12
    readonly property var nmDeviceTypeBridge: 13

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

    property var dbusNetwork: NetworkManager{}
    property var activeConnections: dbusNetwork.activeConnections

    signal needSecretsEmit(string path, string encryptionName, string accessPointName)

    property bool inPasswordInputting: false

    property bool inAllConnectionPage: stackView.depth == 1
    property var allConnectionPage: ListConnections {}

    property var stackViewPages: {
        "allConnectionPage": Qt.resolvedUrl("ListConnections.qml"),
        "infoPage": Qt.resolvedUrl("Info.qml"),
        "connectionPropertiesPage": Qt.resolvedUrl("ConnectionProperties.qml"),
        // TODO remove
        // "wirelessPropertiesPage": Qt.resolvedUrl("WirelessProperties.qml"),
        // "wiredPropertiesPage": Qt.resolvedUrl("WiredProperties.qml")
    }

    function resetConnectionSession(){
        if(stackView.currentItemId == "connectionPropertiesPage"){
            stackView.currentItem.connectionSessionObject.Close()
        }
    }

    function marshalJSON(value){
        var valueJSON = JSON.stringify(value);
        // print("==> unmarshalJSON:", value, valueJSON) // TODO test
        return valueJSON
    }
    
    function unmarshalJSON(valueJSON){
        // print("==> unmarshalJSON:", valueJSON) // TODO test
        var value = JSON.parse(valueJSON)
        return value
    }
    
    Connections{
        target: rootWindow

        onModuleStartChange: {
            resetConnectionSession()
        }

        onPanelHided: {
            resetConnectionSession()
            stackView.reset()
        }
    }

    Component {
        id: connectionSession
        ConnectionSession {}
    }

    DssTitle {
        id:header
        height: 48
        text: dsTr("Network Settings")
        rightLoader.sourceComponent: Row {
            height: header.height
            spacing: 4

            DssAddButton{
                id: addButton
                anchors.verticalCenter: parent.verticalCenter
                // TODO temporary scheme
                property var menuLabels
                function menuSelect(i){
                    print("create connection", menuLabels[i])
                    goToCreateConnection(getSupportedConnectionTypesInfo()[i].Value)
                }
                onClicked: {
                    if(!rootMenu.visible){
                        menuLabels = getSupportedConnectionTypesText() // update menu labels
                        var pos = mapToItem(null, 0, 0)
                        rootMenu.labels = addButton.menuLabels
                        rootMenu.requestMenuItem = addButton
                        rootMenu.innerWidth = 250
                        rootMenu.posX = pos.x - rootMenu.innerWidth + width
                        rootMenu.posY = pos.y + height
                    }
                    rootMenu.visible = !rootMenu.visible
                }
                function getSupportedConnectionTypesInfo() {
                    return JSON.parse(dbusNetwork.GetSupportedConnectionTypes())
                }
                function getSupportedConnectionTypesText() {
                    var connTypesText = []
                    var connTypesInfo = getSupportedConnectionTypesInfo()
                    for (var i=0; i<connTypesInfo.length; i++) {
                        connTypesText.push(connTypesInfo[i].Text)
                    }
                    return connTypesText
                }
                function goToCreateConnection(type){
                    stackView.push({
                        "item": stackViewPages["connectionPropertiesPage"],
                        "properties": { "create": true, "type":  type},
                        "destroyOnPop": true
                    })
                    stackView.currentItemId = "connectionPropertiesPage"
                }
            }

            DTextButton {
                text: "Info"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    stackView.push(stackViewPages["infoPage"])
                    stackView.currentItemId = "infoPage"
                }
            }
        }
    }

    DSeparatorHorizontal{}

    StackView {
        id:stackView
        width: parent.width
        height: parent.height - header.height - 2
        property string currentItemId: ""

        function reset(){
            stackView.pop(null)
            stackView.currentItemId = "allConnectionPage"
        }

        Component.onCompleted: {
            stackView.push(stackViewPages["allConnectionPage"])
            stackView.currentItemId = "allConnectionPage"
        }
    }
}
