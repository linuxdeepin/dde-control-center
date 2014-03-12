import QtQuick 2.0
import Deepin.Widgets 1.0

Rectangle{
    id: monitorComponent

    property int monitorIndex: index
    property var monitorObject: undefined
    property var scaleFactorAndPadding: [1.0, 0, 0]
    property bool inEditMode: true
    property int openedMonitorNumber: 0

    property var identifyWindow: IdentifyWindow{
        monitorObject: monitorComponent.monitorObject
        visible: inEditMode
    }

    signal pressedAction(int monitorIndex)
    signal releasedAction(int monitorIndex)
    signal dragAndMoveAction(int monitorIndex)

    property string displayName: monitorObject.name
    property real scaleFactor: scaleFactorAndPadding[0]
    property real xPadding: scaleFactorAndPadding[1]
    property real yPadding: scaleFactorAndPadding[2]

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

    function reset(){
        x = monitorObject.x * scaleFactor + xPadding
        y = monitorObject.y * scaleFactor + yPadding
        width = monitorObject.width*scaleFactor
        height = monitorObject.height*scaleFactor
    }

    property real x1: x
    property real x2: x+width
    property real y1: y
    property real y2: y+height

    DssH4 {
        id: position
        text: "(" + parseInt(parent.x) + ", " + parseInt(parent.y) + ")"
        visible: pressed
    }

    DssH3 {
        id: nameText
        anchors.centerIn: parent
        text: displayName
    }

    DssH4 {
        text: "Drag me"
        anchors.top: nameText.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        opacity: 0.5
        visible: inEditMode
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
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        text: "Split"
        onClicked: {
            displayId.SplitMonitor(monitorObject.name)
        }
        visible: monitorObject.isComposited
    }

    Item{
        id: setPrimaryButton
        anchors.right: parent.right
        anchors.top: parent.top
        width: stateButton.width + 6
        height: stateButton.height + 6
        visible: openedMonitorNumber > 1

        property bool hovered: false
        property bool selected: displayId.primary == monitorObject.name

        Rectangle{
            id: stateButton
            anchors.centerIn: parent
            width: 16
            height: 16
            radius: 8
            color: {
                if(parent.selected){
                    return dconstants.activeColor
                }
                else{
                    return Qt.rgba(0, 0, 0, 0.3)
                }
            }
            border.width: 1
            border.color: {
                if(parent.selected){
                    return "#1493f2"
                }
                else if(parent.hovered){
                    return dconstants.activeColor
                }
                else{
                    return "white"
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true

            onEntered: {
                parent.hovered = true
            }

            onExited: {
                parent.hovered = false
            }

            onClicked: {
                if(!parent.selected){
                    displayId.SetPrimary(monitorObject.name)
                }
            }
        }
    }

}
