/*************************************************************
*File Name: GeneratingCodeItem.qml
*Author: Match
*Email: Match.YangWanQing@gmail.com
*Created Time: 2015年04月21日 星期二 14时15分39秒
*Description:
*
*************************************************************/
import QtQuick 2.1
import Deepin.Widgets 1.0
import "../Widgets"

Item {

    Rectangle {
        id: contentRec
        width: parent.width
        height: 70
        color: DConstants.contentBgColor

        WaitingImage {
            id: waitingimg
            width: 16
            height: 16
            anchors {right: creatingTipText.left; rightMargin: 5; top: creatingTipText.top;topMargin: 3}
            on: sharePanel.state == "CreatingCode"
        }
        Text {
            id: creatingTipText
            anchors {left: parent.left;leftMargin: 16 + 16 + 5;  verticalCenter: parent.verticalCenter}
            width:  parent.width - 16 * 2 - 5
            height: contentHeight + 10
            lineHeight: 1.3
            font.pixelSize: 10
            color: "#7C7C7C"
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.Wrap
            text: dsTr("Generating the verification code, please wait...")
        }
    }

    DSeparatorHorizontal {
        id:creatingTextSeparator
        anchors.top: contentRec.bottom
    }

    DTextButton {
        text: dsTr("Cancel")
        anchors {right: parent.right; rightMargin: 8; top: creatingTextSeparator.bottom; topMargin: 5}

        onClicked: {
            reset()
            remotingServer.Stop()
        }
    }
}
