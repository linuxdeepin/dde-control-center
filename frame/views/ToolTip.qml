import QtQuick 2.1
import QtQuick.Window 2.1

Window {
    id: toolTip
    width: tipText.width + 10
    height: tipText.height + 10
    flags: Qt.Popup | Qt.WindowStaysOnTopHint
    color: Qt.rgba(0, 0, 0, 0)

    property var cursorPos

    function showTip(s){
        cursorPos = windowView.getCursorPos()
        tipText.text = s
        timeoutShow.restart()
    }

    function hideTip(){
        toolTip.hide()
        timeoutShow.stop()
    }

    Timer {
        id: timeoutShow
        interval: 500
        running: false
        repeat: false
        onTriggered: {
            var pos = toolTip.cursorPos
            toolTip.x = pos[0] + 10
            toolTip.y = pos[1] + 10
            if (toolTip.x + toolTip.width > screenSize.width){
                toolTip.x = screenSize.width - toolTip.width
            }
            if (toolTip.y + toolTip.height > screenSize.height){
                toolTip.y = pos[1] - 10 - toolTip.height
            }
            toolTip.show()
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
