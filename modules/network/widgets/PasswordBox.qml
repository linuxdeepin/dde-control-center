import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import "../../shared"

Item {
    id: passwordArea
    width: parent.width
    height: 0
    clip: true

    property string uuid: ""
    property string path: ""
    property bool prefixCondition: true

    property alias arrowHeight: arrowRectBackground.arrowHeight
    property int realHeight: childrenRect.height
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
            if (uuid){
                if(passwordArea.path == path && prefixCondition){
                    print("onNeedSecretsEmit:", passwordArea.path, path) // TODO test
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
