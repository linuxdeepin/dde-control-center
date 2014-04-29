import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Locale 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0
import "../shared"

Window {
    id: messageBox
    color: "transparent"
    flags: Qt.Tool | Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint

    x: screenSize.x + (screenSize.width - width)/2
    y: screenSize.y + (screenSize.height - height)/2

    width: 300
    height: 120

    function showDialog(){
        rootWindow.clickedToHide = false
        countdown.restart()
        messageBox.show()
    }

    function hideDialog(){
        messageBox.hide()
        countdown.reset()
        rootWindow.clickedToHide = true
    }

    Timer {
        id: countdown
        property int totalTime: 10

        function reset(){
            countdown.stop()
            totalTime = 10
        }

        running: false
        repeat: true
        interval: 1000
        onTriggered: {
            if(totalTime > 1){
                totalTime = totalTime - 1
            }
            else{
                displayId.ResetChanged()
                hideDialog()
            }
        }
    }

    DialogBox {
        id: window
        anchors.fill: parent
        radius: 5

        MouseArea {
            anchors.fill: parent

            property int startX
            property int startY
            property bool holdFlag
            onPressed: {
                startX = mouse.x;
                startY = mouse.y;
                holdFlag = true;
            }
            onReleased: holdFlag = false;
            onPositionChanged: {
                if (holdFlag) {
                    messageBox.setX(messageBox.x + mouse.x - startX)
                    messageBox.setY(messageBox.y + mouse.y - startY)
                }
            }
        }

        DssH1{
            id: messageLabel
            anchors.top: parent.top
            anchors.topMargin: 20
            anchors.left: parent.left
            anchors.leftMargin: 20
            color: "white"
            text: dsTr("Do you want to keep these display settings?")
            font.pixelSize: 14
        }

        DssH3{
            anchors.top: messageLabel.bottom
            anchors.topMargin: 6
            anchors.left: messageLabel.left

            color: dconstants.fgColor
            text: dsTr("Reverting to previous display settings in <font color='#F48914'>%1</font> seconds.").arg(countdown.totalTime)
            font.pixelSize: 12
        }

        Row {
            anchors.right: parent.right
            anchors.rightMargin: 6
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 6
            spacing: 6

            DTransparentButton {
                text: dsTr("Keep changes")
                onClicked: {
                    displayId.SaveChanged()
                    hideDialog()
                }
            }

            DTransparentButton {
                text: dsTr("Revert")
                onClicked: {
                    displayId.ResetChanged()
                    hideDialog()
                }
            }
        }
    }
}
