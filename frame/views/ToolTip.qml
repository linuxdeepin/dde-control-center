import QtQuick 2.1
import QtQuick.Window 2.1

Window {
    id: toolTip
    width: tipText.width + 10
    height: tipText.height + 10
    flags: Qt.Popup | Qt.WindowStaysOnTopHint
    color: Qt.rgba(0, 0, 0, 0)

    function showTip(s){
        tipText.text = s
        timeoutShow.restart()
        freshBackground.restart()
    }

    function hideTip(){
        timeoutShow.stop()
        toolTip.hide()
        toolTip.visible = false
    }

    Timer{
        id: freshBackground
        interval: 200
        repeat: true
        property int repeatTime: 0

        onTriggered: {
            if(repeatTime > 10){
                freshBackground.stop()
                freshBackground.repeat = false
            }
            else{
                repeatTime += 1
                if (background.color == Qt.rgba(0, 0, 0, 0.9)){
                    background.color = Qt.rgba(0, 0, 0, 0.91)
                }
                else{
                    background.color = Qt.rgba(0, 0, 0, 0.9)
                }
            }
        }
    }

    Timer {
        id: timeoutShow
        interval: 500
        running: false
        repeat: false
        onTriggered: {
            toolTip.x = cursorPosition.x + 10
            toolTip.y = cursorPosition.y + 10
            if (toolTip.x + toolTip.width > screenSize.width){
                toolTip.x = screenSize.width - toolTip.width
            }
            if (toolTip.y + toolTip.height > screenSize.height){
                toolTip.y = pos[1] - 10 - toolTip.height
            }
            toolTip.show()
            toolTip.visible = true
            timeoutHide.restart()
        }
    }

    Timer {
        id: timeoutHide
        interval: 3000
        running: false
        repeat: false 
        onTriggered: {
            toolTip.hideTip()
        }
    }

    Rectangle {
        id: background
        width: parent.width
        height: parent.height
        color: Qt.rgba(0, 0, 0, 0.9)
        radius: 4

        Text {
            id: tipText
            anchors.centerIn: parent
            font.pixelSize: 12
            color: "#faca57"
        }
    }
}
