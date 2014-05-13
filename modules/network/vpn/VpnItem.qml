import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import "../../shared"
import "../widgets"

Item {
    id: vpnItem
    width: parent.width
    height: childrenRect.height

    Behavior on height {
        PropertyAnimation { duration: 100 }
    }

    property int connectionStatus: 100

    property var infos: vpnConnections[index]
    property var activeConnectionInfo: getActiveConnectionInfo(infos.Uuid)

    property bool isConnected: activeConnectionInfo && activeConnectionInfo.Vpn && activeConnectionInfo.State == nmActiveConnectionStateActivated

    function goToEditConnection(){
        stackView.push({
            "item": stackViewPages["connectionPropertiesPage"],
            "properties": { "uuid": infos.Uuid, "connectionPath": infos.Path, "devicePath": "/" },
            "destroyOnPop": true
        })
        stackView.currentItemId = "connectionPropertiesPage"
    }

    function activateThisConnection(){
        dbusNetwork.ActivateConnection(infos.Uuid, "/")
    }

    DBaseLine {
        id: lineBox

        property bool hovered: false
        property bool selected: false
        color: dconstants.contentBgColor

        MouseArea{
            z:-1
            anchors.fill:parent
            hoverEnabled: true
            onEntered: parent.hovered = true
            onExited: parent.hovered = false

            onClicked: {
                if (isConnected){
                    goToEditConnection()
                }
                else{
                    activateThisConnection()
                }
            }
        }

        leftLoader.sourceComponent: Item {
            width: parent.width
            height: parent.height

            DImageButton {
                anchors.verticalCenter: parent.verticalCenter
                normal_image: "../images/connected.png"
                hover_image: "../images/disconnect.png"
                visible: isConnected
                onClicked: {
                    dbusNetwork.DeactivateConnection(infos.Uuid)
                }
            }

            WaitingImage {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                on: activeConnectionInfo && activeConnectionInfo.Uuid == infos.Uuid && activeConnectionInfo.Vpn && activeConnectionInfo.State == nmActiveConnectionStateActivating
            }

            DLabel {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                text: infos.Id
                font.pixelSize: 12
                color: {
                    if(lineBox.selected){
                        return dconstants.activeColor
                    }
                    else if(lineBox.hovered){
                        return dconstants.hoverColor
                    }
                    else{
                        return dconstants.fgColor
                    }
                }
            }
        }

        rightLoader.sourceComponent: DArrowButton {
            onClicked: {
                goToEditConnection()
            }
        }
    }

    Item{
        id: passwordArea
        anchors.top: lineBox.bottom
        anchors.topMargin: 0 - arrowRectBackground.arrowHeight
        width: parent.width
        height: 0
        property int realHeight: childrenRect.height
        clip: true

        property string path: ""
        property string encryptionName: ""

        function showArea(encryptionName){
            passwordArea.encryptionName = encryptionName
            height = realHeight
            networkModule.inPasswordInputting = true
            passwordInput.forceActiveFocus()
        }

        function hideArea(){
            height = 0
            networkModule.inPasswordInputting = false
        }

        function cancelAction(){
            hideArea()
            dbusNetwork.CancelSecret(passwordArea.path, passwordArea.encryptionName)
        }

        function connectAction(){
            hideArea()
            dbusNetwork.FeedSecret(passwordArea.path, passwordArea.encryptionName, passwordInput.text)
        }

        Connections {
            target: networkModule
            onNeedSecretsEmit: {
                if (infos.Uuid){
                    passwordArea.path = infos.Path
                    print("onNeedSecretsEmit:", passwordArea.path, path) // TODO test
                    if(passwordArea.path == path){
                        passwordArea.showArea(encryptionName)
                    }
                }
            }
        }

        ArrowRect{
            id: arrowRectBackground
            anchors.fill: parent
            fillStyle: dconstants.bgColor
            stroke: false
            radius: 0
            lineWidth: 0
            arrowPosition: 0.15
        }

        Item {
            id: passwordInputArea
            anchors.top: parent.top
            anchors.topMargin: arrowRectBackground.arrowHeight + 18
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - 48
            height: 38

            DssH2 {
                id: passwordLabel
                anchors.verticalCenter: parent.verticalCenter
                text: dsTr("Password: ")
            }

            DTextInput{
                id: passwordInput
                width: parent.width - passwordLabel.width - 10
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                textInput.color: dconstants.fgColor
                echoMode: TextInput.Password
                onAccepted: {
                    passwordArea.connectAction()
                }
            }
        }

        Row{
            anchors.top: passwordInputArea.bottom
            anchors.topMargin: 6
            anchors.right: parent.right
            anchors.rightMargin: 15
            height: 38
            spacing: 10

            DTextButton{
                text: dsTr("Cancel")
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    passwordArea.cancelAction()
                }
            }

            DTextButton{
                text: dsTr("Connect")
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    passwordArea.connectAction()
                }
            }
        }
    }
}
