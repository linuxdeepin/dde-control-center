/*************************************************************
*File Name: ErrorItem.qml
*Author: Match
*Email: Match.YangWanQing@gmail.com
*Created Time: 2015年04月21日 星期二 14时16分56秒
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

    signal retryGenerateCode
    Rectangle {
        id: contentRec4
        width: parent.width
        height: childrenRect.height
        anchors.top: parent.top
        color: DConstants.contentBgColor

        Text {
            id:errorTipText
            width: parent.width - 40
            height: 70
            lineHeight: 1.3
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.Wrap
            font.pixelSize: 12
            color: DConstants.tuhaoColor
            anchors {top: parent.top; horizontalCenter: parent.horizontalCenter}
            text: dsTr("获取验证码失败，您可以重试获取或者取消操作")
        }
    }


    DSeparatorHorizontal {
        id:errorSeparator
        anchors.top: contentRec4.bottom
    }

    Row {
        anchors {right: parent.right; rightMargin: 8; top: errorSeparator.bottom; topMargin: 5}
        spacing: 5

        DTextButton {
            text: dsTr("Cancel")

            onClicked: {
                reset()
                remotingServer.Stop()
            }
        }

        DTextButton {
            text: dsTr("Retry")

            onClicked: {
                errorItem.retryGenerateCode()
                remotingServer.Start()
            }
        }
    }
}
