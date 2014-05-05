import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Locale 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0
import "widgets"

Window {
    id: messageBox
    color: "transparent"
    flags: Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint

    x: screenSize.x + (screenSize.width - width)/2
    y: screenSize.y + (screenSize.height - height)/2

    property var displayId: Display {}
    property var screenSize: QtObject {
        property int x: displayId.primaryRect[0]
        property int y: displayId.primaryRect[1]
        property int width: displayId.primaryRect[2]
        property int height: displayId.primaryRect[3]
    }

    property string accessPointName: "linuxdepin-1"
    property string accessPointEncryption: ""
    property var accessPointObj: undefined
    signal connect(string password)
    signal cancel()

    width: 480
    height: 210

    function showDialog(){
        messageBox.show()
    }

    function hideDialog(){
        messageBox.hide()
    }

    function cancelAction(){
        hideDialog()
        dbusNetwork.CancelSecret(accessPointObj, accessPointEncryption)
    }

    function connectAction(){
        hideDialog()
        dbusNetwork.FeedSecret(accessPointObj, accessPointEncryption, passwordInput.text)
    }

    DWindowFrame {
        id: window
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent

            property int startX
            property int startY
            property bool holdFlag
            onPressed: {
                startX = mouse.x;
                startY = mouse.y;
                holdFlag = true;
            }
            onReleased: holdFlag = false;
            onPositionChanged: {
                if (holdFlag) {
                    messageBox.setX(messageBox.x + mouse.x - startX)
                    messageBox.setY(messageBox.y + mouse.y - startY)
                }
            }
        }

        CloseButton {
            id: closeButton
            anchors.right: parent.right
            onClicked: {
                cancelAction()
            }
        }

        Item{
            width: parent.width - 30
            anchors.top: parent.top
            anchors.topMargin: closeButton.height
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.right: parent.right
            anchors.rightMargin: 20

            DssH1{
                id: title
                text: dsTr("Authentication required by Wi-Fi network")
                font.bold: true
            }

            DssH2{
                id: message
                anchors.top: title.bottom
                anchors.topMargin: 6
                text: dsTr('Passwords or encryption keys are required to access the Wi-Fi network <font color="#0ff">%1</font>').arg(accessPointName)
                width: parent.width
                wrapMode: Text.WordWrap
            }


            Row {
                height: 30
                width: childrenRect.width
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: message.bottom
                anchors.topMargin: 10
                spacing: 10
                DssH2{
                    text: dsTr("Password:")
                    anchors.top: parent.top
                    anchors.topMargin: 2
                }

                Column{
                    DTextInput{
                        id: passwordInput
                        width: 250
                        echoMode: showPasswordButton.checked ? TextInput.Normal : TextInput.Password
                    }

                    Row{
                        width: parent.width
                        spacing: 10

                        DSwitchButton{
                            id: showPasswordButton
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        DssH3{
                            text: dsTr("Show Password")
                            anchors.verticalCenter: parent.verticalCenter
                        }

                    }
                }
            }

        }

        Row {
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            spacing: 8

            DTextButton {
                text: dsTr("Cancel")
                onClicked: {
                    cancelAction()
                }
            }

            DTextButton {
                text: dsTr("Connect")
                onClicked: {
                    connectAction()
                }
            }
        }
    }
}
