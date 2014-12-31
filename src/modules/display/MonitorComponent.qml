/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Kaisheng Ye
**
**  Author:     Kaisheng Ye <kaisheng.ye@gmail.com>
**  Maintainer: Kaisheng Ye <kaisheng.ye@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

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
        else{
            return dconstants.fgDarkColor
        }
    }
    visible: monitorObject.opened
    opacity: pressed ? 0.6 : 0.9

    x: monitorObject.x * scaleFactor + xPadding
    y: monitorObject.y * scaleFactor + yPadding
    width: monitorObject.width * scaleFactor
    height: monitorObject.height * scaleFactor

    Binding{
        target: monitorComponent
        property: "x"
        value: monitorObject.x * scaleFactor + xPadding
        when: !inEditMode
    }
    Binding{
        target: monitorComponent
        property: "y"
        value: monitorObject.y * scaleFactor + yPadding
        when: !inEditMode
    }
    Binding{
        target: monitorComponent
        property: "width"
        value: monitorObject.width * scaleFactor
        when: !inEditMode
    }
    Binding{
        target: monitorComponent
        property: "height"
        value: monitorObject.height * scaleFactor
        when: !inEditMode
    }

    function reset(){
        x = monitorObject.x * scaleFactor + xPadding
        y = monitorObject.y * scaleFactor + yPadding
        width = monitorObject.width * scaleFactor
        height = monitorObject.height * scaleFactor
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
        anchors.centerIn: realMonitorsCount == 1 ? parent : undefined
        anchors.right: realMonitorsCount > 1 ? parent.right : undefined
        anchors.rightMargin: realMonitorsCount > 1 ? 10 : 0
        anchors.top: realMonitorsCount > 1 ? parent.top: undefined
        anchors.topMargin: realMonitorsCount > 1 ? 10 : 0
        text: displayName
        font.weight: Font.DemiBold
        font.pixelSize: parent.width/12
        visible: !(monitorObject.isComposited && inEditMode)
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
                rootWindow.setPanelHidable(false)
            }
        }
        onReleased: {
            if(inEditMode){
                parent.pressed = false
                releasedAction(monitorIndex)
                rootWindow.setPanelHidable(true)
            }
        }
        onPositionChanged: {
            if(inEditMode){
                dragAndMoveAction(monitorIndex)
            }
        }
    }

    Image {
        id:dockShadowImg
        source: "images/dock-shadow.png"
        width: parent.width * 2 / 5
        height: parent.height / 10
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        visible: !monitorObject.isComposited && displayId.primary == monitorObject.name && openedMonitors.length > 1
    }

}
