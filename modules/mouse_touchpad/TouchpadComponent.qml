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

Item {
    id: keyboardModule
    width: parent.width
    height: childrenRect.height

    property int contentLeftMargin: 22
    property int contentHeight: 48
    property int sliderWidth: 180

    visible: dbusTouchpad.exist

    Column {
        id: touchpadTitleColumn
        anchors.top: parent.top
        width: parent.width

        DBaseLine {
            id: touchpadTitle
            leftLoader.sourceComponent: DssH2 {
                color: "white"
                font.bold: true
                text: dsTr("Touchpad")
            }
            rightLoader.visible: !autoDisableTouchpad.enabled
            rightLoader.sourceComponent: DSwitchButton{
                checked: dbusTouchpad.tPadEnable
                onClicked: {
                    dbusTouchpad.tPadEnable = checked
                }
                visible: dbusMouse.exist
            }
        }

        DSeparatorHorizontal {}
    }

    Column {
        id: touchpadSettingColumn
        anchors.top: touchpadTitleColumn.bottom
        width: parent.width
        clip: true
        property int realHeight: childrenRect.height

        height: touchpadTitle.rightLoader.item.checked ? realHeight : 0

        Behavior on height {
            PropertyAnimation { duration: 100 }
        }

        DCenterLine {
            height: 38
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Primary Button")
            content.sourceComponent: DRadioButton {
                id: leftHandedSelectButton
                width: 180
                anchors.left: parent.left
                anchors.leftMargin: 2
                anchors.verticalCenter: parent.verticalCenter
                initializeIndex: dbusTouchpad.leftHanded ? 1 : 0
                buttonModel: [
                    {"buttonId": "right_hand", "buttonLabel": dsTr("Left Button")},
                    {"buttonId": "left_hand", "buttonLabel": dsTr("Right Button")}
                ]

                onItemSelected: {
                    dbusTouchpad.leftHanded = (idx == 1)
                }

                Connections {
                    target: dbusTouchpad
                    onLeftHandedChanged: {
                        if(dbusTouchpad.leftHanded && leftHandedSelectButton.currentIndex == 0){
                            leftHandedSelectButton.selectItem(1)
                        }
                        else if(!dbusTouchpad.leftHanded && leftHandedSelectButton.currentIndex == 1){
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
                init: dbusTouchpad.motionAcceleration
                valueDisplayVisible: false

                onValueConfirmed:{
                    dbusTouchpad.motionAcceleration = value
                }

                Component.onCompleted: {
                    addRuler(min, dsTr("Slow"))
                    addRuler(max, dsTr("Fast"))
                }

                Connections {
                    target: dbusTouchpad
                    onMotionAccelerationChanged: {
                        if(!equal(dbusTouchpad.motionAcceleration, pointerSpeedSlider.value, 0.01)){
                            pointerSpeedSlider.setValue(dbusTouchpad.motionAcceleration, false)
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
                init: dbusTouchpad.doubleClick
                valueDisplayVisible: false

                onValueConfirmed:{
                    dbusTouchpad.doubleClick = value
                }

                Component.onCompleted: {
                    addRuler(1000, dsTr("Slow"))
                    addRuler(100, dsTr("Fast"))
                }

                Connections {
                    target: dbusTouchpad
                    onDoubleClickChanged: {
                        if(!equal(dbusTouchpad.doubleClick, doubleClickSpeedSlider.value, 1)){
                            doubleClickSpeedSlider.setValue(dbusTouchpad.doubleClick, false)
                        }
                    }
                }

            }

        }

        DCenterLine {
            height: contentHeight
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Drag Threshold")

            content.sourceComponent: DSliderEnhanced {
                id: dragThresholdSlider
                width: sliderWidth

                min: 1
                max: 10
                init: dbusTouchpad.dragThreshold
                valueDisplayVisible: false

                onValueConfirmed:{
                    dbusTouchpad.dragThreshold = value
                }

                Component.onCompleted: {
                    addRuler(1, dsTr("Short"))
                    addRuler(10, dsTr("Long"))
                }

                Connections {
                    target: dbusTouchpad
                    onDragThresholdChanged: {
                        if(!equal(dbusTouchpad.dragThreshold, dragThresholdSlider.value, 0.1)){
                            dragThresholdSlider.setValue(dbusTouchpad.dragThreshold, false)
                        }
                    }
                }

            }
        }

        DCenterLine {
            height: contentHeight
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Natural Scrolling")

            content.sourceComponent: Item {
                width: sliderWidth
                height: parent.height

                DSwitchButton {
                    id: naturalScrollButton
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    checked: dbusTouchpad.naturalScroll

                    MouseArea {
                        anchors.fill: parent
                        onClicked: dbusTouchpad.naturalScroll = !dbusTouchpad.naturalScroll
                    }
                }
            }
        }

        DCenterLine {
            height: contentHeight
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Tap to click")

            content.sourceComponent: Item {
                width: sliderWidth
                height: parent.height

                DSwitchButton {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    checked: dbusTouchpad.tapClick

                    MouseArea {
                        anchors.fill: parent
                        onClicked: dbusTouchpad.tapClick = !dbusTouchpad.tapClick
                    }
                }
            }
        }

        DCenterLine {
            height: contentHeight
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Two-finger scrolling")

            content.sourceComponent: Item {
                width: sliderWidth
                height: parent.height

                DSwitchButton {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    checked: dbusTouchpad.vertScroll

                    MouseArea {
                        anchors.fill: parent
                        onClicked: dbusTouchpad.vertScroll = !dbusTouchpad.vertScroll
                    }
                }
            }
        }

        DCenterLine {
            height: contentHeight
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Edge scrolling")

            content.sourceComponent: Item {
                width: sliderWidth
                height: parent.height

                DSwitchButton {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    checked: dbusTouchpad.edgeScroll

                    MouseArea {
                        anchors.fill: parent
                        onClicked: dbusTouchpad.edgeScroll = !dbusTouchpad.edgeScroll
                    }
                }
            }
        }

        DSeparatorHorizontal {}
    }
}
