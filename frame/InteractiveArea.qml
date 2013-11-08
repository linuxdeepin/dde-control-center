import QtQuick 2.1

MouseArea {
    onEntered: {
        inInteractiveArea = true
    }
    
    onExited: {
        inInteractiveArea = false
    }
}
