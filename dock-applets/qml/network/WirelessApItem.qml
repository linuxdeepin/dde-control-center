import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0

Item {
    id: wirelessItem
    width: parent.width
    height: 30

    property string connectionPath
    property string uuid
    property bool apConnected: isActiveConnection(devicePath, uuid) && deviceStatus == 100

    property bool hovered: false
    property bool selected: false

    MouseArea{
        anchors.fill:parent
        hoverEnabled: true
        onEntered: parent.hovered = true
        onExited: parent.hovered = false
        onClicked: {
            if(!apConnected){
                print("activate connection")
                activateThisConnection()
            }
        }
    }

    Item {
        height: parent.height
        width: parent.width

        DImageButton {
            anchors.right: nameLabel.left
            anchors.rightMargin: 4
            anchors.verticalCenter: parent.verticalCenter
            normal_image: "images/check_1.png"
            hover_image: "images/check_2.png"
            visible: {
                if(inConnectingApPath != "/"){
                    return false
                }
                else{
                    return apConnected
                }
            }
            onClicked: {
                // disconnect connection
                dbusNetwork.DisconnectDevice(wirelessItem.devicePath)
            }
        }

        WaitingImage {
            anchors.right: nameLabel.left
            anchors.rightMargin: 4
            anchors.verticalCenter: parent.verticalCenter
            on: inConnectingApPath == apPath
        }

        DLabel {
            id: nameLabel
            anchors.left: parent.left
            anchors.leftMargin: 24
            anchors.verticalCenter: parent.verticalCenter
            verticalAlignment: Text.AlignVCenter
            text: apName
            elide: Text.ElideRight
            width: parent.width - anchors.leftMargin - signalImage.width - signalImage.anchors.rightMargin
            font.pixelSize: 12
            color: {
                if(selected){
                    return dconstants.activeColor
                }
                else if(hovered){
                    return dconstants.hoverColor
                }
                else{
                    return dconstants.fgColor
                }
            }
        }
    }

    Image {
        id: signalImage
        anchors.right: parent.right
        anchors.rightMargin: 15
        anchors.verticalCenter: parent.verticalCenter
        source: {
            var power = apSignal
            var secure = apSecured ? "-secure": ""
            if (power <= 5)
                return "images/ap-signal-0" + secure + ".svg"
            else if (power <= 25)
                return "images/ap-signal-25" + secure + ".svg"
            else if (power <= 50)
                return "images/ap-signal-50" + secure + ".svg"
            else if (power <= 75)
                return "images/ap-signal-75" + secure + ".svg"
            else if (power <= 100)
                return "images/ap-signal-100" + secure + ".svg"
        }
    }

    Behavior on height {
        PropertyAnimation { duration: 100 }
    }

    Connections {
        target: dbusNetwork
        onConnectionsChanged: {
            updateWirelessConnectionInfo()
        }
    }
    Component.onCompleted: {
        updateWirelessConnectionInfo()
    }
    
    function updateWirelessConnectionInfo() {
        if(nmConnections){
            var wirelessConnections = nmConnections["wireless"]
            var connectionPath = ""
            var uuid = ""
            for (var i in wirelessConnections) {
                if (apName == wirelessConnections[i].Ssid) {
                    if (wirelessConnections[i].HwAddr == "" || wirelessConnections[i].HwAddr == deviceHwAddr) {
                        connectionPath = wirelessConnections[i].Path
                        uuid = wirelessConnections[i].Uuid
                        print("update connection:", wirelessConnections[i].Ssid, uuid, connectionPath, deviceHwAddr) // TODO test
                        break
                    }
                }
            }
            wirelessItem.connectionPath = connectionPath
            wirelessItem.uuid = uuid
        }
    }
    
    function activateThisConnection(){
        if (apSecuredInEap && uuid == "") {
            // if security in eap, go to edit connection
            print("secured in eap") // TODO debug
            //goToEditConnection()
        } else {
            dbusNetwork.ActivateAccessPoint(apPath, devicePath)
            inConnectingApPath = apPath
        }
    }

    function doGoToEditConnection(){
        stackView.push({
            "item": stackViewPages["connectionPropertiesPage"],
            "properties": { "uuid": wirelessItem.uuid, "devicePath": wirelessItem.devicePath },
            "destroyOnPop": true
        })
        stackView.currentItemId = "connectionPropertiesPage"
    }

    function doGoToCreateConnection(){
        stackView.push({
            "item": stackViewPages["connectionPropertiesPage"],
            "properties": { "create": true, "type":  nmConnectionTypeWireless},
            "destroyOnPop": true
        })
        stackView.currentItemId = "connectionPropertiesPage"
    }
}
