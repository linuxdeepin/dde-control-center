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

    function showError(){
        needCodeItem.state = "error"
    }

    Rectangle {
        id: contentRec1
        width: parent.width
        height: childrenRect.height
        anchors.top: parent.top
        color: DConstants.contentBgColor

        TextInput {
            id: codeInput
            font.pixelSize: 30
            color: DConstants.activeColor
            horizontalAlignment: TextInput.AlignHCenter
            verticalAlignment: TextInput.AlignVCenter
            width: parent.width
            height: 70
            selectByMouse: true
            anchors {top: parent.top; horizontalCenter: parent.horizontalCenter}
            Component.onCompleted: forceActiveFocus()
            validator: RegExpValidator{
                regExp: /[A-Za-z0-9]{6}/
            }
            onTextChanged: {
                if (codeInput.length === 6)
                    needCodeItem.state = "gotContent"
                else
                    needCodeItem.state = "inputting"
            }
        }

        Rectangle {
            width: parent.width - 15 * 2
            height: 1
            color: DConstants.fgDarkColor
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: codeInput.bottom
        }

        Text {
            id: codeText
            width: contentWidth
            height: 20
            verticalAlignment: Text.AlignTop
            anchors {top: codeInput.bottom; topMargin: 10; horizontalCenter: parent.horizontalCenter}
            font.pixelSize: 10
            color: "#7C7C7C"
            text: dsTr("Please enter the verification code in the input field above")
        }

    }

    DSeparatorHorizontal {
        id:codeSeparator
        anchors.top: contentRec1.bottom
    }

    Row {
        spacing: 3
        anchors {right: parent.right; rightMargin: 8; top: codeSeparator.bottom; topMargin: 5}

        DTextButton {
            text: dsTr("Cancel")

            onClicked: {
                reset()
            }
        }

        DTextButton {
            id: connectButton
            enabled: needCodeItem.state === "gotContent"
            text: dsTr("Connect")
            textColor: inactiveColor

            onClicked: {
                print ("Connect to:", codeInput.text)
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
                text: dsTr("Please input access token")
                color: "#7C7C7C"
            }
        },
        State {
            name: "error"
            PropertyChanges {
                target: codeText
                text: dsTr("Acess token is invalid, please retry")
                color: DConstants.tuhaoColor
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
                text: dsTr("Start after clicking on \“Connect\”")
                color: "#7C7C7C"
            }
            PropertyChanges {
                target: connectButton
                textColor: DConstants.textNormalColor
            }
        }

    ]
}
