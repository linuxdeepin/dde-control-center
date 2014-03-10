import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Widgets 1.0

Window {
    id: identifyWindow
    color: "transparent"
    flags: Qt.Popup | Qt.WindowStaysOnTopHint
    width: monitorName.width + 8
    height: monitorName.height + 8

    property var monitorObject
    property alias timeoutHide: timeoutHide

    x: monitorObject.x
    y: monitorObject.y

    Rectangle {
        anchors.fill: parent
        color: "yellow"
    }

    DLabel {
        id: monitorName
        anchors.centerIn: parent
        text: monitorObject.name
        font.pixelSize: monitorObject.width/70
        font.bold: true
        color: "black"
    }

    Timer {
        id: timeoutHide
        running: false
        interval: 2500
        repeat: false
        onTriggered: {
            identifyWindow.hide()
        }
    }
}
