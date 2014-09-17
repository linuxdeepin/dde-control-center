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
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.InputDevices 1.0
import "../shared"

Item {
    id: mouseModule
    anchors.fill: parent

    property int contentLeftMargin: 22
    property int contentHeight: 48
    property int sliderWidth: 180
    property int leftWidth: 100
    property int centerPadding: 16

    property var dbusMouse: Mouse {}

    function equal(a, b, minValue){
        return Math.abs(a-b) < minValue
    }

    Column {
        id: mouseSettingsColumn
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text: dsTr("Mouse")

            rightLoader.sourceComponent: ResetButton {
                onClicked: {
                    dbusMouse.Reset()
                    if (dbusTouchpad.exist){
                        dbusTouchpad.Reset()
                    }
                }
            }
        }

        DSeparatorHorizontal {}

        DCenterLine {
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Primary Button")
            content.sourceComponent: DRadioButton {
                id: leftHandedSelectButton
                anchors.left: parent.left
                anchors.leftMargin: 2
                anchors.verticalCenter: parent.verticalCenter
                initializeIndex: dbusMouse.leftHanded ? 1 : 0
                buttonModel: [
                    {"buttonId": "right_hand", "buttonLabel": dsTr("Left Button")},
                    {"buttonId": "left_hand", "buttonLabel": dsTr("Right Button")}
                ]

                onItemSelected: {
                    dbusMouse.leftHanded = idx == 1 ? true : false
                }

                Connections {
                    target: dbusMouse
                    onLeftHandedChanged: {
                        if(dbusMouse.leftHanded && leftHandedSelectButton.currentIndex == 0){
                            leftHandedSelectButton.selectItem(1)
                        }
                        else if(!dbusMouse.leftHanded && leftHandedSelectButton.currentIndex == 1){
                            leftHandedSelectButton.selectItem(0)
                        }
                    }
                }
            }
        }

        DCenterLine {
            height: contentHeight
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Pointer Speed")

            content.sourceComponent: DSliderEnhanced {
                id: pointerSpeedSlider
                width: sliderWidth

                min: 3
                max: 0.2
                init: dbusMouse.motionAcceleration
                valueDisplayVisible: false

                onValueConfirmed:{
                    dbusMouse.motionAcceleration = value
                }

                Component.onCompleted: {
                    addRuler(min, dsTr("Slow"))
                    addRuler(max, dsTr("Fast"))
                }

                Connections {
                    target: dbusMouse
                    onMotionAccelerationChanged: {
                        if(!equal(dbusMouse.motionAcceleration, pointerSpeedSlider.value, 0.01)){
                            pointerSpeedSlider.setValue(dbusMouse.motionAcceleration, false)
                        }
                    }
                }
            }

        }

        DCenterLine {
            height: contentHeight
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Double-click Speed")

            content.sourceComponent: DSliderEnhanced {
                id: doubleClickSpeedSlider
                width: sliderWidth

                min: 1000
                max: 100
                init: dbusMouse.doubleClick
                valueDisplayVisible: false

                onValueConfirmed:{
                    dbusMouse.doubleClick = value
                }

                Component.onCompleted: {
                    addRuler(1000, dsTr("Slow"))
                    addRuler(100, dsTr("Fast"))
                }

                Connections {
                    target: dbusMouse
                    onDoubleClickChanged: {
                        if(!equal(dbusMouse.doubleClick, doubleClickSpeedSlider.value, 1)){
                            doubleClickSpeedSlider.setValue(dbusMouse.doubleClick, false)
                        }
                    }
                }
            }

        }

        DCenterLine {
            id: autoDisableTouchpad
            height: Math.max(contentHeight, title.height + 6)
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Disable the touchpad when inserting the mouse")
            visible: dbusTouchpad.exist

            property bool enabled: {
                if(content.item){
                    return content.item.enabled
                }
                else{
                    return false
                }
            }

            content.sourceComponent: Item {
                width: sliderWidth
                height: parent.height
                property alias enabled: disableTouch.checked

                DSwitchButton {
                    id: disableTouch
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    checked: dbusMouse.disableTpad

                    MouseArea{
                        anchors.fill: parent
                        onClicked: dbusMouse.disableTpad = !dbusMouse.disableTpad
                    }

                }
            }

        }

        DSeparatorHorizontal {}

        DBaseLine{ height: 26 }

        TouchpadComponent {}
    }
}
