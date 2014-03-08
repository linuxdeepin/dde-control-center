import QtQuick 2.0
import Deepin.Widgets 1.0

Rectangle{
    id: monitorComponent

    property int monitorIndex: index
    property var monitorObject: undefined
    property var scaleFactorAndPadding: [1.0, 0, 0]
    property bool inEditMode: true

    signal pressedAction(int monitorIndex)
    signal releasedAction(int monitorIndex)
    signal dragAndMoveAction(int monitorIndex)

    property string displayName: monitorObject.name
    property real scaleFactor: scaleFactorAndPadding[0]
    property int xPadding: scaleFactorAndPadding[1]
    property int yPadding: scaleFactorAndPadding[2]

    property bool pressed: false
    property bool beJoined: false
    property bool beOverlapped: false

    color: pressed ? "#252525" : "#0d0d0d"
    border.width: 1
    border.color: beJoined ? "red" : dconstants.fgDarkColor
    visible: monitorObject.opened
    opacity: pressed ? 0.6 : 0.9

    x: monitorObject.x * scaleFactor + xPadding
    y: monitorObject.y * scaleFactor + yPadding
    width: monitorObject.width*scaleFactor
    height: monitorObject.height*scaleFactor

    property int x1: parseInt(x)
    property int x2: parseInt(x+width)
    property int y1: parseInt(y)
    property int y2: parseInt(y+height)

    DssH4 {
        id: position
        anchors.top: parent.top
        anchors.right: parent.left
        text: "(" + parseInt(parent.x) + ", " + parseInt(parent.y) + ")"
        visible: pressed
    }

    DssH3 {
        text: monitorObject.width + "x" + monitorObject.height
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
        visible: inEditMode
        cursorShape: {
            if(inEditMode){
                if(parent.pressed){
                    return Qt.ClosedHandCursor
                }
                else{
                    return Qt.OpenHandCursor
                }
            }
            else{
                return Qt.ArrowCursor
            }
        }
        onPressed: {
            parent.pressed = true
            pressedAction(monitorIndex)
        }
        onReleased: {
            parent.pressed = false
            releasedAction(monitorIndex)
        }
        onPositionChanged: {
            dragAndMoveAction(monitorIndex)
        }
    }

    DTextButton{
        anchors.centerIn: parent
        text: "Split"
        onClicked: {
            displayId.SplitMonitor(monitorObject.name)
        }
        visible: monitorObject.isComposited
    }

}
