import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Widgets 1.0

Window {
    id: identifyWindow
    color: "transparent"
    flags: Qt.Popup | Qt.WindowStaysOnTopHint
    width: monitorName.width + 20
    height: monitorName.height + 20

    property var monitorObject

    x: monitorObject.x + (monitorObject.width - identifyWindow.width)/2
    y: monitorObject.y + (monitorObject.height - identifyWindow.height)/2

    function showWindow(){
        identifyWindow.showNormal()
        timeoutHide.restart()
    }

    DLabel {
        id: monitorName
        anchors.centerIn: parent
        text: monitorObject.name
        font.pixelSize: monitorObject.width/8
        font.bold: true
        color: "white"
        style: Text.Outline; styleColor: "black"
    }

    Timer {
        id: timeoutHide
        running: false
        interval: 2200
        repeat: false
        onTriggered: {
            identifyWindow.hide()
        }
    }
}
