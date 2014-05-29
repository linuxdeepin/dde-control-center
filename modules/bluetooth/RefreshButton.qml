import QtQuick 2.1

RotatingImage {
    
    Timer {
        id: stop_rotating_timer
        interval: 10 * 1000
        onTriggered: { rotating = false }
    }
    
    MouseArea {
        anchors.fill: parent
        
        onClicked: {
            rotating = true
            stop_rotating_timer.restart()
        }
    }
}