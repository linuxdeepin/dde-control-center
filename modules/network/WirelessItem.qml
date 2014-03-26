import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import "../shared/"

Item {
    width: parent.width
    height: childrenRect.height

    Behavior on height {
        PropertyAnimation { duration: 100 }
    }

    DBaseLine {
        id: wirelessLine

        property bool hovered: false
        property bool selected: false

        MouseArea{
            z:-1
            anchors.fill:parent
            hoverEnabled: true
            visible: !networkModule.inPasswordInputting

            onEntered: {
                parent.hovered = true
                //print(accessPoints[index])
            }

            onExited: {
                parent.hovered = false
            }

            onClicked: {
                nm.ActiveAccessPoint(dev[0], accessPoints[index][3])
            }
        }
        color: dconstants.contentBgColor

        leftLoader.sourceComponent: Item {
            height: parent.height
            width: childrenRect.width

            DImageButton {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                normal_image: "img/check_1.png"
                hover_image: "img/check_2.png"
                visible: accessPoints[index][4]
                onClicked: {
                    nm.DisconnectDevice(dev[0])
                }
            }

            WaitingImage {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
            }

            DLabel {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
                text: accessPoints[index][0]
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
                    var power=  accessPoints[index][2]
                    var secure = accessPoints[index][1] ? "-secure": ""
                    if (power <= 5)
                        return "img/ap-signal-0" + secure + ".svg"
                    else if (power <= 25)
                        return "img/ap-signal-25" + secure + ".svg"
                    else if (power <= 50)
                        return "img/ap-signal-50" + secure + ".svg"
                    else if (power <= 75)
                        return "img/ap-signal-75" + secure + ".svg"
                    else if (power <= 100)
                        return "img/ap-signal-100" + secure + ".svg"
                }
            }

            DArrowButton {
                onClicked: {
                    stackView.push({
                        "item": Qt.resolvedUrl("WirelessProperties.qml"),
                        "properties": { "accessPoint": accessPoints[index]},
                        "destroyOnPop": true
                    })
                }
            }
        }
    }

    Item{
        id: passwordArea
        anchors.top: wirelessLine.bottom
        anchors.topMargin: 0 - arrowRectBackground.arrowHeight
        width: parent.width
        height: 0
        property int realHeight: childrenRect.height
        clip: true

        property string path: nm.GetConnectionByAccessPoint(accessPoints[index][3])[1]
        property string encryptionName: ""

        function showArea(encryptionName){
            encryptionName = encryptionName
            height = realHeight
            networkModule.inPasswordInputting = true
        }

        function cancelAction(){
            height = 0
            networkModule.inPasswordInputting = false
        }

        function connectAction(){
            cancelAction()
            dbusNetwork.FeedSecret(path, encryptionName, passwordInput.text)
        }

        Connections {
            target: networkModule
            onNeedSecretsEmit: {
                if(passwordArea.path == path){
                    passwordArea.showArea(encryptionName)
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

        Row{
            id: passwordInputArea
            anchors.top: parent.top
            anchors.topMargin: arrowRectBackground.arrowHeight + 18
            anchors.left: parent.left
            anchors.leftMargin: 30
            height: childrenRect.height
            spacing: 10

            DssH2 {
                text: "Password: "
            }

            Column{
                DTextInput{
                    id: passwordInput
                    echoMode: showPasswordButton.checked ? TextInput.Normal : TextInput.Password
                }
                Row{
                    spacing: 6

                    DSwitchButton{
                        id: showPasswordButton
                    }

                    DssH3{
                        text: "Show password"
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }
        }

        Row{
            anchors.top: passwordInputArea.bottom
            anchors.right: parent.right
            anchors.rightMargin: 15
            height: childrenRect.height
            spacing: 10

            DTextButton{
                text: "Cancel"
                onClicked: {
                    passwordArea.cancelAction()
                }
            }

            DTextButton{
                text: "Connect"
                onClicked: {
                    passwordArea.connectAction()
                }
            }
        }
    }

    DSeparatorHorizontal{
        anchors.top: wirelessLine.bottom
        anchors.topMargin: passwordArea.height == 0 ? 0 : passwordArea.height - arrowRectBackground.arrowHeight
    }
}
