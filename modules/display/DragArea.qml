import QtQuick 2.1

MouseArea {
    id: dragArea
    property variant window
    
    property bool isHover: false
    property bool isDoubleClick: false
    
    property real lastX: 0
    property real lastY: 0
    property real lastWindowX: 0
    property real lastWindowY: 0
    
    signal singleClicked
    
    onPressed: {
        isHover = false
        isDoubleClick = false
        
        var pos = window.getCursorPos()
        lastX = pos.x
        lastY = pos.y
        
        lastWindowX = window.x
        lastWindowY = window.y
    }
    
    onClicked: {
        if (!isHover) {
            clickTimer.restart()
        }
    }
    
    onDoubleClicked: {
        isDoubleClick = true
    }
    
    onPositionChanged: {
        isHover = true
        
        if (pressedButtons == Qt.LeftButton) {
            var pos = window.getCursorPos()
            window.x = lastWindowX + pos.x - lastX
            window.y = lastWindowY + pos.y - lastY
        }
    }
    
    Timer {
        id: clickTimer
        interval: 200
        repeat: false
        onTriggered: {
            if (!dragArea.isDoubleClick) {
                singleClicked()
            }
        }
    }
}
