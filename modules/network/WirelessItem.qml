import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import "../shared/"
import "widgets"

Item {
    id: wirelessItem
    width: parent.width
    height: childrenRect.height

    property string devicePath: "/"
    property string deviceHwAddress: ""
    property string activeAp: "/"
    property int deviceStatus: 0
    property string connectionPath
    property string uuid
    property bool apConnected: activeAp == apPath && deviceStatus == 100

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
        var wirelessConnections = nmConnections[nmConnectionTypeWireless]
        var connectionPath = ""
        var uuid = ""
        for (var i in wirelessConnections) {
            if (apName == wirelessConnections[i].Ssid) {
                if (wirelessConnections[i].HwAddress == "" || wirelessConnections[i].HwAddress == deviceHwAddress) {
                    connectionPath = wirelessConnections[i].Path
                    uuid = wirelessConnections[i].Uuid
                    break
                }
            }
        }
        wirelessItem.connectionPath = connectionPath
        wirelessItem.uuid = uuid
    }
    
    function activateThisConnection(){
        if (uuid == "" && apSecuredInEap) {
            // if access point under the security eap and there is
            // no valid connection for it, go to connection
            // property page and create connection session through
            // CreateConnectionForAccessPoint()
            goToCreateConnection()
        } else {
            // connection for current access point exists, just activate it
            print("connectionPath", connectionPath)
            dbusNetwork.ActivateAccessPoint(apPath, wirelessItem.devicePath)
        }
    }

    function editThisConnection(){
        if (uuid != "") {
            print("goToEditConnection", uuid)
            goToEditConnection()
        } else {
            print("goToCreateConnection")
            goToCreateConnection()
        }
    }

    function goToEditConnection(){
        stackView.push({
            "item": stackViewPages["connectionPropertiesPage"],
            "properties": { "uuid": wirelessItem.uuid, "devicePath": wirelessItem.devicePath },
            "destroyOnPop": true
        })
        stackView.currentItemId = "connectionPropertiesPage"
    }

    function goToCreateConnection(){
        stackView.push({
            "item": stackViewPages["connectionPropertiesPage"],
            "properties": { "createForAp": true, "apPath": apPath, "devicePath": wirelessItem.devicePath},
            "destroyOnPop": true
        })
        stackView.currentItemId = "connectionPropertiesPage"
    }
    
    DBaseLine {
        id: wirelessLine

        property bool hovered: false
        property bool selected: false

        MouseArea {
            z: -1
            anchors.fill:parent
            hoverEnabled: true
            visible: !networkModule.inPasswordInputting

            onEntered: {
                parent.hovered = true
            }

            onExited: {
                parent.hovered = false
            }

            onClicked: {
                if(!apConnected){
                    print("activate connection") // TODO debug
                    activateThisConnection()
                }
                else{
                    print("edit connection") // TODO debug
                    editThisConnection()
                }
            }
        }
        color: dconstants.contentBgColor

        leftLoader.sourceComponent: Item {
            height: parent.height
            width: childrenRect.width

            DImageButton {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                normal_image: "images/connected.png"
                hover_image: "images/disconnect.png"
                visible: apConnected
                onClicked: {
                    dbusNetwork.DisconnectDevice(wirelessItem.devicePath)
                }
            }

            WaitingImage {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                on: activeAp == apPath && deviceStatus >= nmDeviceStatePrepare && deviceStatus <= nmDeviceStateSecondaries
            }

            DLabel {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
                text: apName
                font.pixelSize: 12
                color: {
                    if(wirelessLine.selected){
                        return dconstants.activeColor
                    }
                    else if(wirelessLine.hovered){
                        return dconstants.hoverColor
                    }
                    else{
                        return dconstants.fgColor
                    }
                }
            }
        }

        rightLoader.sourceComponent: Row {
            height: childrenRect.height
            anchors.verticalCenter: parent.verticalCenter
            spacing: 8

            Image {
                source: {
                    var power = apSignal
                    var secure = apSecured ? "-secure": ""
                    var imageNumber = 100

                    if (power <= 5){
                        imageNumber = 0
                    }
                    else if (power <= 25) {
                        imageNumber = 25
                    }
                    else if (power <= 50) {
                        imageNumber = 50
                    }
                    else if (power <= 75) {
                        imageNumber = 75
                    }
                    else if (power <= 100) {
                        imageNumber = 100
                    }

                    return "images/wifi-signal-" + imageNumber + secure + ".svg"
                }
            }

            DArrowButton {
                onClicked: {
                    editThisConnection()
                }
            }
        }
    }

    PasswordBox {
        id: passwordArea
        anchors.top: wirelessLine.bottom
        anchors.topMargin: 0 - arrowHeight
        uuid: wirelessItem.uuid
        path: wirelessItem.connectionPath
    }
}
