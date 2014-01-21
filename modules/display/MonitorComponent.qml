import QtQuick 2.0
import Deepin.Widgets 1.0

Rectangle{
    id: monitorComponent

    property var outputObj
    property int monitorIndex

    property string displayName: outputObj.name
    property var modeInfo: outputObj.mode

    property real whFactor: modeInfo[1]/modeInfo[2]

    width: 120
    height: width/whFactor
    color: "#0d0d0d"
    border.width: 1
    border.color: dconstants.fgDarkColor
    visible: outputObj.opened

    DssH3 {
        anchors.centerIn: parent
        text: modeInfo[1]+ "x" + modeInfo[2] + "x" + modeInfo[3]
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
        //drag.target: parent
        //drag.axis: Drag.XAndYAxis
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
