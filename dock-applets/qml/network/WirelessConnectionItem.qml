import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0

Item {
    id: wirelessItem
    width: parent.width
    height: childrenRect.height

    property string devicePath: "/"
    property string inConnectingApPath: "/"
    property string uuid: dbusNetwork.GetConnectionUuidByAccessPoint(apPath)
    property bool apConnected: getIndexFromArray(uuid, activeConnections) != -1

    function requestUuid(){
        // TODO get uuid each time to fix connection issue temporary
        var uuid = dbusNetwork.GetConnectionUuidByAccessPoint(apPath)
        print("uuid=",uuid, uuid=="")
        if(uuid == ""){
            uuid = dbusNetwork.CreateConnectionForAccessPoint(apPath)
            print("create connection, uuid=",uuid)
        }
        if(uuid != ""){
            wirelessItem.uuid = uuid
        }
        if(wirelessItem.uuid == ""){
            wirelessItem.uuid = dbusNetwork.CreateConnectionForAccessPoint(apPath)
        }
    }

    function activateThisConnection(){
        wirelessItem.uuid = dbusNetwork.ActivateConnectionForAccessPoint(apPath, wirelessItem.devicePath)
        inConnectingApPath = apPath
    }

    DBaseLine {
        id: wirelessLine

        property bool hovered: false
        property bool selected: false
        color: "transparent"

        leftLoader.sourceComponent: Item {
            height: parent.height
            width: childrenRect.width

            DImageButton {
                anchors.left: parent.left
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
                    // TODO
                    // dbusNetwork.DeactivateConnection(uuid)
                    dbusNetwork.DeleteConnection(uuid)
                }
            }

            WaitingImage {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                on: inConnectingApPath == apPath
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

        rightLoader.sourceComponent: Image {
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

        MouseArea{
            z:-1
            anchors.fill:parent
            hoverEnabled: true

            onEntered: {
                parent.hovered = true
            }

            onExited: {
                parent.hovered = false
            }

            onClicked: {
                // if security in eap, go to edit connection always
                if(apSecuredInEap) {
                    print("secured in eap") // TODO debug
                    goToEditConnection()
                }
                else{
                    if(!apConnected){
                        print("activate connection") // TODO debug
                        activateThisConnection()
                    }
                    else{
                        print("edit connection") // TODO debug
                        goToEditConnection()
                    }
                }
            }
        }

    }
}
