/*************************************************************
*File Name: ConnectedItem.qml
*Author: Match
*Email: Match.YangWanQing@gmail.com
*Created Time: 2015年04月21日 星期二 14时16分41秒
*Description:
*
*************************************************************/
import QtQuick 2.1
import Deepin.Widgets 1.0

Item {

    Rectangle {
        id: contentRec3
        width: parent.width
        height: childrenRect.height
        anchors.top: parent.top
        color: DConstants.contentBgColor

        Text {
            id: connectedTipText
            width: parent.width - 40
            height: 70
            lineHeight: 1.3
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.Wrap
            font.pixelSize: 12
            color: "#7C7C7C"
            anchors {top: parent.top; horizontalCenter: parent.horizontalCenter}
            text: dsTr("Your desktop is sharing, you can continue to share or select to disconnect")
        }
    }


    DSeparatorHorizontal {
        id:connectedSeparator
        anchors.top: contentRec3.bottom
    }

    DTextButton {
        text: dsTr("Disconnect")
        anchors {right: parent.right; rightMargin: 8; top: connectedSeparator.bottom; topMargin: 5}

        onClicked: {
            remotingServer.Stop()
            reset()
        }
    }
}
