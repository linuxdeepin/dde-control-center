/*************************************************************
*File Name: ErrorItem.qml
*Author: Match
*Email: Match.YangWanQing@gmail.com
*Created Time: 2015年04月21日 星期二 15时20分31秒
*Description:
*
*************************************************************/
import QtQuick 2.1
import Deepin.Widgets 1.0

Item {
    id: errorItem

    function setErrorMessage(message){
        errorTipText.text = message
    }

    signal retryConnect

    Rectangle {
        id: contentRec4
        width: parent.width
        height: childrenRect.height
        anchors.top: parent.top
        color: DPalette.contentBgColor

        Text {
            id: errorTipText
            width: parent.width - 40
            height: 70
            lineHeight: 1.3
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.Wrap
            font.pixelSize: 12
            color: DPalette.tuhaoColor
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            text: dsTr("Failed to establish the connection, you can retry to connect ")
        }
    }


    DSeparatorHorizontal {
        id: errorSeparator
        anchors.top: contentRec4.bottom
    }

    Row {
        anchors {
            right: parent.right
            rightMargin: 8
            top: errorSeparator.bottom
            topMargin: 5
        }
        spacing: 5

        DTextButton {
            text: dsTr("Cancel")

            onClicked: {
                resetPage()
            }
        }

        DTextButton {
            text: dsTr("Retry")

            onClicked: {
                remotingClient.Start()
                errorItem.retryConnect()
            }
        }
    }
}

