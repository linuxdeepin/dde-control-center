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
    function dsTr(s) { return s; }

    Rectangle {
        id: mainContainer
        width: parent.width
        height:  100
        color: DPalette.contentBgColor

        Column {
            id: layout
            anchors.fill: parent
            anchors.leftMargin: 15
            anchors.rightMargin: 15

            Item {
                width: parent.width
                height: 70

                Row {
                    anchors.centerIn: parent
                    width: waitingimg.width + waitingTip.width
                    spacing: 10

                    WaitingImage {
                        id: waitingimg
                        width: 16
                        height: 16
                    }

                    Text {
                        id: waitingTip
                        width: contentWidth
                        height: waitingimg.height
                        font.pixelSize: 10
                        color: "#7C7C7C"
                        verticalAlignment: Text.AlignVCenter
                        text: dsTr("Establishing connection, please wait...")
                    }
                }
            }

            Rectangle {
                height: 1
                width: parent.width
                //color: DPalette.fgDarkColor
                color: "#7C7C7C"
            }

            Item {
                height: 10
                width: parent.width
            }

            Text {
                id: waitingText
                width: parent.width
                height: 20
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 10
                color: "#7C7C7C"
                text: dsTr("Share window would be opend when connect is done")
            }
        }
    }

    Item {
        id: buttonLayout
        width: parent.width
        height: cancelButton.height
        anchors.top: mainContainer.bottom
        anchors.topMargin: 6
        anchors.right: parent.right
        anchors.rightMargin: 15

        DTextButton {
            id: cancelButton
            text: dsTr("Cancel")
            anchors.right: parent.right

            onClicked: {
                remotingClient.Stop()
                resetPage()
            }
        }
    }
}
