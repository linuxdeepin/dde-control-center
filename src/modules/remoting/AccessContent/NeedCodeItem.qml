/*************************************************************
*File Name: NeedCodeItem.qml
*Author: Match
*Email: Match.YangWanQing@gmail.com
*Created Time: 2015年04月21日 星期二 15时19分03秒
*Description:
*
*************************************************************/
import QtQuick 2.1
import Deepin.Widgets 1.0

Item {
    id: needCodeItem
    state: "inputting"

    property alias code: codeInput.text
    property color inactiveColor: "#515151"

    function showError(msg){
        needCodeItem.state = "error"
        codeText.text = msg
    }

    function filterPeerId() {
        codeInput.text = codeInput.text.trim().substring(0, 6)
    }

    property var peerIdRegExp: /[A-Za-z0-9]{6}/

    function validatePeerId(peerId) {
        return peerIdRegExp.test(codeInput.text)
    }

    Rectangle {
        id: contentRec1
        width: parent.width
        height: childrenRect.height
        anchors.top: parent.top
        color: DPalette.contentBgColor

        TextInput {
            id: codeInput
            font.pixelSize: 30
            color: DPalette.activeColor
            horizontalAlignment: TextInput.AlignHCenter
            verticalAlignment: TextInput.AlignVCenter
            width: parent.width
            height: 70
            selectByMouse: true
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            Component.onCompleted: forceActiveFocus()

            //validator: RegExpValidator{
            //    regExp: peerIdRegExp
            //}
            Keys.onReturnPressed: {
                if (needCodeItem.state === "gotContent") {
                    connectButton.clicked()
                    return
                }
            }
            onTextChanged: {
                filterPeerId()
                if (validatePeerId()) {
                    needCodeItem.state = "gotContent"
                } else {
                    needCodeItem.state = "inputting"
                }
            }

            MouseArea {
                acceptedButtons: Qt.RightButton
                anchors.fill: parent
                onClicked: {
                    codeInput.text = ''
                    codeInput.paste()
                }
            }
        }

        Rectangle {
            width: parent.width - 15 * 2
            height: 1
            color: DPalette.fgDarkColor
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: codeInput.bottom
        }

        Text {
            id: codeText
            width: contentWidth
            height: 20
            verticalAlignment: Text.AlignTop
            anchors {
                top: codeInput.bottom
                topMargin: 10
                horizontalCenter: parent.horizontalCenter
            }
            font.pixelSize: 10
            color: "#7C7C7C"
            text: dsTr("Please enter the verification code in the input field above")
        }

    }

    DSeparatorHorizontal {
        id: codeSeparator
        anchors.top: contentRec1.bottom
    }

    Row {
        spacing: 3
        anchors {
            right: parent.right
            rightMargin: 8
            top: codeSeparator.bottom
            topMargin: 5
        }

        DTextButton {
            text: dsTr("Cancel")

            onClicked: {
                resetPage()
            }
        }

        DTextButton {
            id: connectButton
            enabled: needCodeItem.state === "gotContent"
            text: dsTr("Connect")
            textColor: inactiveColor

            onClicked: {
                remotingClient.Start()
                accessPanel.state = "Connecting"
            }
        }
    }

    states: [
        State {
            name: "inputting"
            PropertyChanges {
                target: codeText
                text: dsTr("Please enter the verification code in the input field above")
                color: "#7C7C7C"
            }
        },
        State {
            name: "error"
            PropertyChanges {
                target: codeText
                color: DPalette.tuhaoColor
            }
            PropertyChanges {
                target: connectButton
                textColor: inactiveColor
            }
        },
        State {
            name: "gotContent"
            PropertyChanges {
                target: codeText
                text: dsTr("Start remote access after clicking on \"Connect\"")
                color: "#7C7C7C"
            }
            PropertyChanges {
                target: connectButton
                textColor: DPalette.textNormalColor
            }
        }

    ]
}
