import QtQuick 2.1
import QtGraphicalEffects 1.0
import Deepin.Widgets 1.0

DTextInput{
    property int cursorBlinkTime: 1200

    textInput.cursorDelegate: Rectangle {
        width: 1
        height: parent.height
        color: dconstants.fgColor
        opacity: cursorBlinkTimer.cursorOpacity
    }

    Timer{
        id: cursorBlinkTimer
        repeat: true
        running: true
        interval: parent.cursorBlinkTime/2
        property real cursorOpacity: 1

        onTriggered: {
            if(cursorOpacity == 1){
                cursorOpacity = 0
            }
            else{
                cursorOpacity = 1
            }
        }
    }
}
