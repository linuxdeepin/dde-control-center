import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Locale 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0

Window {
    id: messageBox
    color: "transparent"
    flags: Qt.Popup | Qt.WindowStaysOnTopHint

    x: screenSize.x + (screenSize.width - width)/2
    y: screenSize.y + (screenSize.height - height)/2

    width: 300
    height: 140

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

    DWindowFrame {
        id: window
        anchors.fill: parent

        Column{
            width: parent.width - 30
            anchors.centerIn: parent
            spacing: 6

            DssH1{
                color: "white"
                text: dsTr("是否要保存这些显示设置？")
                anchors.horizontalCenter: parent.horizontalCenter
            }

            DssH3{
                color: "white"
                text: dsTr("<font color='red'>%1</font> 秒之后还原之前的显示设置。").arg(countdown.totalTime)
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        Row {
            anchors.right: parent.right
            anchors.rightMargin: 6
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 6

            DTextButton {
                text: dsTr("Save Changes")
                onClicked: {
                    displayId.SaveChanged()
                    hideDialog()
                }
            }

            DTextButton {
                text: dsTr("Restore")
                onClicked: {
                    displayId.ResetChanged()
                    hideDialog()
                }
            }
        }
    }
}
