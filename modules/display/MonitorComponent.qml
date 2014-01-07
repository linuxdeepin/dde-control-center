import QtQuick 2.0
import Deepin.Widgets 1.0

Rectangle{
    id: monitorComponent

    width: 120
    height: 90

    property string displayName: "Unknown"
    property string monitorId: "LVDS1"

    signal pressed(string monitorId)
    signal release(string monitorId)

    color: "#0d0d0d"
    border.width: 1
    border.color: dconstants.fgDarkColor

    DssH2 {
        anchors.centerIn: parent
        text: displayName
    }

    MouseArea {
        anchors.fill: parent
        drag.target: parent
        drag.axis: Drag.XAndYAxis
        onPressed: {
            parent.color = "#252525"
            parent.opacity = 0.6
            monitorComponent.pressed(monitorId)
        }
        onReleased: {
            parent.color = "#0d0d0d"
            monitorComponent.pressed(monitorId)
        }
    }

}
