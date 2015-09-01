/*************************************************************
*File Name: GeneratedCodeItem.qml
*Author: Match
*Email: Match.YangWanQing@gmail.com
*Created Time: 2015年04月21日 星期二 14时16分07秒
*Description:
*
*************************************************************/
import QtQuick 2.1
import Deepin.Widgets 1.0

Item {

    function setCodeText(value){
        codeText.text = value
    }

    // Display `copy-to-clipboard` tooltip
    function popupCopyTip() {
        copyTip.visible = true
        copyTipTimer.start()
    }

    Rectangle {
        id: contentRec2
        width: parent.width
        height: childrenRect.height
        anchors.top: parent.top
        color: DPalette.contentBgColor

        TextEdit {
            id: codeText
            enabled: false
            width: parent.width
            height: 50
            font.pixelSize: 24
            color: DPalette.activeColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            text: ""
        }

        Text {
            id: tipText
            width: parent.width - 16 * 2
            height: contentHeight + 10
            lineHeight: 1.3
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.Wrap
            font.pixelSize: 10
            color: "#7C7C7C"
            anchors {
                top: codeText.bottom
                topMargin: 10
                horizontalCenter: parent.horizontalCenter
            }
            text: dsTr("To start sharing your desktop, please provide the above verification code to whom will assist you. Your shared session will begin immediately after verification code input")
        }
    }

    DSeparatorHorizontal {
        id: createdTextSeparator
        anchors.top: contentRec2.bottom
    }

    Row {
        id: buttonBox
        anchors {
            right: parent.right
            rightMargin: 8
            top: createdTextSeparator.bottom
            topMargin: 5
        }
        spacing: 5

        DTextButton {
            text: dsTr("Cancel")

            onClicked: {
                remotingServer.Stop()
                resetPage()
            }
        }

        DTextButton {
            text: dsTr("Copy Code")

            onClicked: {
                codeText.selectAll()
                codeText.copy()
                codeText.deselect()
                popupCopyTip()
            }
        }
    }

    Text {
        id: copyTip
        enabled: true
        visible: false
        anchors.top: buttonBox.bottom
        anchors.topMargin: 5
        anchors.leftMargin: 5
        color: '#ffc735'
        font.pixelSize: 10
        text: dsTr('Copied to clipboard successfully')
    }

    Timer {
        id: copyTipTimer
        repeat: false
        interval: 3000
        onTriggered: {
            copyTip.visible = false
        }
    }
}
