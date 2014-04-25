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
    border.color: {
        if(inEditMode && beJoined){
            return "red"
        }
        else if(!inEditMode && monitorChoose.currentSelectedMonitor == monitorObject && openedMonitorNumber > 1){
            return dconstants.activeColor
        }
        else{
            return dconstants.fgDarkColor
        }
    }
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

    DssH1 {
        id: nameText
        anchors.centerIn: parent
        text: displayName
        font.bold: true
        font.pixelSize: parent.width/12
        visible: !monitorObject.isComposited && !inEditMode
    }

    Item {
        id: compositedMonitorBox
        anchors.centerIn: parent
        width: parent.width - 40
        height: parent.height - 60
        property var monitorNames: monitorObject.outputs
        property int childrenWidth: compositedMonitorBox.width/4 * 3
        property int childrenHeight: compositedMonitorBox.height/4 * 3
        property int xSpacing: (width - childrenWidth)/(monitorNames.length - 1)
        property int ySpacing: (height - childrenHeight)/(monitorNames.length - 1)

        Repeater{
            model: compositedMonitorBox.monitorNames
            delegate: Rectangle {
                width: compositedMonitorBox.childrenWidth
                height: compositedMonitorBox.childrenHeight
                border.width: 1
                border.color: dconstants.fgDarkColor
                color: "black"
                x: compositedMonitorBox.xSpacing * index
                y: compositedMonitorBox.ySpacing * index

                DssH3 {
                    text: modelData
                    anchors.right: parent.right
                }

                DssH1 {
                    text: "A"
                    font.pixelSize: parent.width/3
                    anchors.centerIn: parent
                }
            }
        }
        visible: monitorObject.isComposited
    }

    MouseArea {
        anchors.fill: parent
        drag.target: inEditMode ? parent : undefined
        drag.axis: Drag.XAndYAxis
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
            if(inEditMode){
                parent.pressed = true
                pressedAction(monitorIndex)
            }
        }
        onReleased: {
            if(inEditMode){
                parent.pressed = false
                releasedAction(monitorIndex)
            }
        }
        onPositionChanged: {
            if(inEditMode){
                dragAndMoveAction(monitorIndex)
            }
        }

        onClicked: {
            if(!inEditMode){
                monitorChoose.rightLoader.item.selectItemFromId(monitorObject)
            }
        }
    }

    DTextAction{
        anchors.right: parent.right
        text: "Split"
        onClicked: {
            displayId.SplitMonitor(monitorObject.name)
        }
        visible: monitorObject.isComposited
        hasUnderline: true
    }

    Item{
        id: setPrimaryButton
        anchors.right: parent.right
        anchors.top: parent.top
        width: stateButton.width + 6
        height: stateButton.height + 6
        visible: openedMonitorNumber > 1 && !inEditMode

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
                if(parent.selected || parent.hovered){
                    return  dconstants.activeColor
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
                if(!parent.selected){
                    toolTip.showTip(dsTr("Select as Main Monitor"))
                }
            }

            onExited: {
                parent.hovered = false
                toolTip.hideTip()
            }

            onClicked: {
                if(!parent.selected){
                    displayId.SetPrimary(monitorObject.name)
                }
            }
        }
    }

}
