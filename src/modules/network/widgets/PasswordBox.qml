import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Network 1.0
import "../../shared"

Item {
    id: passwordArea
    width: parent.width
    height: 0
    clip: true

    property string path: ""
    property bool prefixCondition: true

    property alias arrowHeight: arrowRectBackground.arrowHeight
    property int realHeight: childrenRect.height
    property string encryptionName: ""
    property bool autoconnect: false
    property bool cacheCheckState: false

    function showArea(encryptionName,autoconnect){
        passwordArea.encryptionName = encryptionName
        passwordArea.autoconnect = autoconnect
        autoConnectBox.checked = autoconnect
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
        print ("==> ConnectAction...",passwordArea.path,passwordArea.encryptionName,passwordInput.text,passwordArea.autoconnect)
        dbusNetwork.FeedSecret(passwordArea.path, passwordArea.encryptionName, passwordInput.text, passwordArea.autoconnect)
    }

    function cleanPassword(){
        passwordInput.text = ""
    }

    Connections {
        target: networkModule
        onNeedSecretsEmit: {
            if (path == passwordArea.path) {
                if(prefixCondition){
                    print("==> onNeedSecretsEmit:", passwordArea.path, passwordArea.prefixCondition)
                    passwordArea.showArea(encryptionName,autoconnect)
                }
            }
        }
    }

    ArrowRect {
        id: arrowRectBackground
        anchors.fill: parent
        fillStyle: DPalette.bgColor
        stroke: false
        radius: 0
        lineWidth: 0
        arrowPosition: 0.15
    }

    DTextInput{
        id: passwordInput
        width: 250
        anchors.top: parent.top
        anchors.topMargin: arrowRectBackground.arrowHeight + 18
        anchors.horizontalCenter: parent.horizontalCenter
        textInput.color: "#b4b4b4"
        echoMode: TextInput.Password
        onAccepted: {
            passwordArea.connectAction()
        }

        Text {
            anchors.left: parent.left
            anchors.leftMargin: 6
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width / 2
            height: parent.height
            color: "#636363"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            text: dsTr("Password")
            visible: passwordInput.text == ""
        }
    }

    DCheckBox {
        id:autoConnectBox
        text:dsTr("Auto-connect")
        anchors.top: passwordInput.bottom
        anchors.topMargin: 6
        anchors.left: passwordInput.left
        onCheckedChanged: passwordArea.autoconnect = checked
    }

    Row{
        id:buttonRow
        anchors.top: autoConnectBox.bottom
        anchors.topMargin: 0
        anchors.right: passwordInput.right
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
