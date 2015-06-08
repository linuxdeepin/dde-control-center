/*************************************************************
*File Name: ConnectingItem.qml
*Author: Match
*Email: Match.YangWanQing@gmail.com
*Created Time: 2015年04月21日 星期二 15时20分21秒
*Description:
*
*************************************************************/
import QtQuick 2.1
import Deepin.Widgets 1.0
import "../Widgets"

Item {

    Rectangle {
        id: contentRec2
        width: parent.width
        height: childrenRect.height
        anchors.top: parent.top
        color: DConstants.contentBgColor

        Item {
            id: connectingStateItem
            width: parent.width
            height: 70

            WaitingImage {
                id: waitingimg
                width: 16
                height: 16
                anchors {
                    left: parent.left
                    leftMargin: parent.width / 3
                    verticalCenter: parent.verticalCenter
                }
            }

            Text {
                anchors {
                    left: waitingimg.right
                    leftMargin: 10
                    verticalCenter: waitingimg.verticalCenter
                }
                width: waitingimg.width
                height: waitingimg.height
                font.pixelSize: 10
                color: "#7C7C7C"
                verticalAlignment: Text.AlignVCenter
                text: dsTr("Establishing connection, please wait...")
            }
        }


        Rectangle {
            width: parent.width - 15 * 2
            height: 1
            color: DConstants.fgDarkColor
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: connectingStateItem.bottom
        }

        Text {
            id: waitingText
            width: parent.width - 16 * 2
            height: 20
            verticalAlignment: Text.AlignTop
            horizontalAlignment: Text.AlignHCenter
            anchors {
                top: connectingStateItem.bottom
                topMargin: 10
                horizontalCenter: parent.horizontalCenter
            }
            font.pixelSize: 10
            color: "#7C7C7C"
            text: dsTr("Share window would be opend when connect is done")
        }
    }

    DSeparatorHorizontal {
        id:connectingSeparator
        anchors.top: contentRec2.bottom
    }

    DTextButton {
        text: dsTr("Cancel")
        anchors {
            right: parent.right
            rightMargin: 8
            top: connectingSeparator.bottom
            topMargin: 5
        }

        onClicked: {
            remotingClient.Stop()
            resetPage()
        }
    }
}
