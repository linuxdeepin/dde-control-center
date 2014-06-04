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

import QtQuick 2.1
import Deepin.Widgets 1.0
import QtGraphicalEffects 1.0

FocusScope {
    id: root
    width: 76
    height: 60
    state: "normal"

    property alias text: text_input.text
    property int textInputRightMargin: 0
    property int textInputLeftMargin: 0
    property variant constants: DConstants {}
    property alias input: text_input
    property int radius: 6

    property int min: 0
    property int max: 23

    signal accepted
    signal toNext

    function increase(){
        var value = parseInt(text_input.text)
        if(value == max){
            text_input.text = min
        }
        else{
            value += 1
            text_input.text = value
        }
    }

    function decrease(){
        var value = parseInt(text_input.text)
        if(value == min){
            text_input.text = max
        }
        else{
            value -= 1
            text_input.text = value
        }
    }

    states: [
        State {
            name: "normal"
            PropertyChanges {
                target: text_input_box_border
                border.color: "black"
            }
        },
        State {
            name: "warning"
            PropertyChanges {
                target: text_input_box_border
                border.color: "#F48914"
            }
        }
    ]

    Rectangle {
        id: text_input_box

        width: parent.width
        height: parent.height
        clip: true
        color: "#171717"
        radius: root.radius
    }

    DropShadow {
        anchors.fill: text_input_box
        radius: 1
        samples: 10
        horizontalOffset: 0
        verticalOffset: 1
        color: Qt.rgba(1, 1, 1, 0.15)
        source: text_input_box
    }

    InnerShadow {
        anchors.fill: text_input_box
        radius: 1
        samples: 10
        horizontalOffset: 0
        verticalOffset: radius
        color: "black"
        source: text_input_box
    }

    Item {
        clip: true
        anchors.fill: text_input_box
        anchors.leftMargin: root.textInputLeftMargin
        anchors.rightMargin: root.textInputRightMargin

        TextInput {
            id: text_input

            focus: true
            verticalAlignment: TextInput.AlignVCenter
            horizontalAlignment: TextInput.AlignHCenter
            maximumLength: 2
            validator: IntValidator{bottom: min; top: max;}
            color: activeFocus ? dconstants.activeColor : dconstants.fgColor
            font.pixelSize: 44
            cursorDelegate: Rectangle{ color: "transparent" }
            selectionColor: "transparent"
            selectedTextColor: dconstants.activeColor

            anchors.fill: parent
            anchors.leftMargin: 3
            anchors.rightMargin: 3

            onActiveFocusChanged: {
                if(activeFocus){
                    selectAll()
                }
                else{
                    deselect()
                }
            }

            onTextChanged: {
                // TODO: add > 100 support
                if(length == 1){
                    if(root.max < 10){
                        //root.toNext()
                    }
                    else if(root.max < 100){
                        var number2 = parseInt(root.max/10)
                        if(parseInt(text) > number2){
                            text = "0" + text
                            //root.toNext()
                        }
                    }
                }
                else if(length == 2){
                    //root.toNext()
                }
            }

            onAccepted: {
                root.accepted()
            }

            Keys.onDownPressed:{
                root.decrease()
            }

            Keys.onUpPressed: {
                root.increase()
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    parent.forceActiveFocus()
                }
                onWheel: {
                    if(wheel.angleDelta.y > 0){
                        root.increase()
                    }
                    else{
                        root.decrease()
                    }
                }
            }

        }
    }

    Rectangle {
        id: text_input_box_border
        radius: root.radius
        color: "transparent"

        anchors.fill:text_input_box
    }

    //MouseArea {
        //anchors.fill: parent
        //onPressed: {
            //mouse.accepted = false
            //text_input.color = constants.fgColor
            //if (root.state == "warning") {
                //root.state = "normal"
            //}
        //}
    //}
}

