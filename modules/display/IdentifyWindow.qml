import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Widgets 1.0

Window {
    id: identifyWindow
    color: "transparent"
    flags: Qt.Tool | Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint
    width: monitorName.width + 20
    height: monitorName.height + 20

    property var monitorObject

    function showWindow(){
        identifyWindow.show()
        repaintTimer.restart()
    }

    x: monitorObject.x
    y: monitorObject.y

    Rectangle {
        anchors.fill: parent
        color: "yellow"
        border.width: 2
        border.color: "black"
    }

    DLabel {
        id: monitorName
        anchors.centerIn: parent
        text: monitorObject.name
        font.pixelSize: monitorObject.width/50
        font.bold: true
        color: "black"
        opacity: 0.8
    }

    Timer {
        id: repaintTimer
        running: false
        interval: 300
        repeat: true
        onTriggered: {
            if(monitorName.opacity < 1){
                monitorName.opacity += 0.1
            }
            else{
                repaintTimer.stop()
            }
        }
    }
}
