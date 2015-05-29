/*************************************************************
*File Name: ConnectedItem.qml
*Author: Match
*Email: Match.YangWanQing@gmail.com
*Created Time: 2015年04月21日 星期二 15时19分33秒
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
            id: connectedText
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            height: 70
            lineHeight: 1.3
            font.pixelSize: 10
            color: "#7C7C7C"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            text: dsTr("You are accessing the desktop shared by other users, you can choose to continue or disconnect")
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
            reset()
            remotingClient.Stop()
        }
    }
}
