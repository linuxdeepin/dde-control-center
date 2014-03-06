import QtQuick 2.0
import Deepin.Widgets 1.0

Rectangle{
    id: monitorComponent

    property var monitorObject: monitorObj
    property int monitorIndex: index

    property string displayName: monitorObject.name
    property real scaleFactor: scaleFactorAndPadding[0]
    property int xPadding: scaleFactorAndPadding[1]
    property int yPadding: scaleFactorAndPadding[2]

    width: monitorObject.width*scaleFactor
    height: monitorObject.height*scaleFactor
    x: monitorObject.x * scaleFactor + xPadding
    y: monitorObject.y * scaleFactor + yPadding
    color: "#0d0d0d"
    border.width: 1
    border.color: dconstants.fgDarkColor
    visible: monitorObject.opened

    Component.onCompleted: {
    }

    DssH3 {
        anchors.centerIn: parent
        text: modeInfo[1]+ "x" + modeInfo[2]
    }

    DssH3 {
        anchors.right: parent.right
        anchors.rightMargin: 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 2
        text: displayName
    }

    MouseArea {
        anchors.fill: parent
        drag.target: parent
        drag.axis: Drag.XAndYAxis
        onPressed: {
            parent.color = "#252525"
            parent.opacity = 0.6
            presentCurrent(monitorIndex)
        }
        onReleased: {
            parent.color = "#0d0d0d"
            releaseAction(monitorIndex)
        }
    }
}
