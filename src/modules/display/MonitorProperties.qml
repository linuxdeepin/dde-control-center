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
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import Deepin.Widgets 1.0
import "../shared"

Item {
    id: monitorProperties
    width: parent.width
    height: visible ? childrenRect.height : 0

    property int sliderWidth: 186
    property var listModelComponent: DListModelComponent {}

    property var initExpanded: true

    property var rotationNames: {
        1: dsTr("Normal"),
                2: dsTr("Rotate Right"),
                4: dsTr("Upside Down"),
                8: dsTr("Rotate Left"),
    }

    property var brightnessValues: displayId.brightness

    property int expandIndex: openedMonitors.length > 1 ? 0 : 2

    Column {
        id: enabledColumn
        width: parent.width
        height: childrenRect.height

        Item {
            width: parent.width
            height: propertyColumn.height
            clip: true

            Column{
                id: propertyColumn
                clip: true
                width: parent.width
                height: childrenRect.height

                DBaseExpand {
                    id:powerArea
                    visible: allMonitorsObjectsCount > 1
                    expanded:  visible && expandIndex == 0
                    onExpandedChanged: {
                        if(header.item){
                            header.item.active = expanded
                        }
                    }
                    header.sourceComponent: DDownArrowHeader {
                        text: dsTr("Enable Monitor(s)")
                        active: expandIndex == 0
                        onClicked: {
                            if(expandIndex == 0){
                                expandIndex = -1
                            }
                            else{
                                expandIndex = 0
                            }
                        }
                    }

                    content.sourceComponent: Item {
                        width: parent.width
                        height: childrenRect.height + 10

                        DssH3 {
                            id: enableTitle
                            text: dsTr("Please choose the monitor(s) you want to enable")
                            color: dconstants.fgDarkColor
                            width: parent.width
                            height: 20
                            anchors.top: parent.top
                            anchors.topMargin: 10
                            anchors.left: parent.left
                            anchors.leftMargin: 15
                        }

                        ListView {
                            id: powerView
                            width: parent.width
                            height: count * 28
                            anchors.top:enableTitle.bottom
                            anchors.horizontalCenter: parent.horizontalCenter

                            model: allMonitorsObjectsCount

                            delegate: EnableSelectItem {
                                pOutputObj:allMonitorsObjects[index]
                                pOutputObjName: pOutputObj.name
                                visible: !pOutputObj.isComposited
                                onSelectAction: {
                                    if (powerView.count > 1){
                                        var nextOpenedState = !pOutputObj.opened
                                        if (!nextOpenedState && openedMonitors.length <= 1)
                                            return
                                        pOutputObj.SwitchOn(nextOpenedState)
                                        powerView.updateEnableMonitorCount()
                                    }
                                }

                            }
                        }
                    }
                }

                DSeparatorHorizontal{visible: powerArea.visible}

                DBaseExpand {
                    id:primaryArea
                    visible: allMonitorsObjectsCount > 1
                    expanded:  visible && expandIndex == 1
                    onExpandedChanged: {
                        if(header.item){
                            header.item.active = expanded
                        }
                    }
                    header.sourceComponent: DDownArrowHeader {
                        text: dsTr("Primary Screen")
                        hintText:"  " + displayId.primary
                        active: expandIndex == 1
                        onClicked: {
                            if(expandIndex == 1){
                                expandIndex = -1
                            }
                            else{
                                expandIndex = 1
                            }
                        }

                    }

                    content.sourceComponent: ListView {
                        id: primaryScreenView
                        width: parent.width
                        height: childrenRect.height

                        model: allMonitorsObjectsCount

                        delegate: PrimaryScreenSelectItem {
                            primaryScreenName: displayId.primary
                            pOutputObj:allMonitorsObjects[index]
                            pOutputObjName: pOutputObj.name
                            visible: !pOutputObj.isComposited
                            onSelectAction: {
                                displayId.SetPrimary(pOutputObjName)
                            }
                        }
                    }
                }

                DSeparatorHorizontal {visible: primaryArea.visible}

                DBaseExpand {
                    id: resolutionArea
                    expanded:  expandIndex == 2
                    onExpandedChanged: {
                        if(header.item){
                            header.item.active = expanded
                        }
                    }
                    header.sourceComponent: DDownArrowHeader {
                        text: dsTr("Resolution")
                        active: expandIndex == 2
                        onClicked: {
                            if(expandIndex == 2){
                                expandIndex = -1
                            }
                            else{
                                expandIndex = 2
                            }
                        }
                    }

                    content.sourceComponent: ListView {
                        id: resolutionView
                        width: parent.width
                        height: childrenRect.height
                        spacing: 5

                        model: allMonitorsObjectsCount

                        delegate: ResolutionRectangle {
                            pOutputObj:allMonitorsObjects[index]
                            pOutputObjName: pOutputObj.name
                            shouldShowMonitorName: pOutputObj.opened && openedMonitors.length > 1
                            itemIndex: index
                        }
                    }

                }

                DSeparatorHorizontal {}

                DBaseExpand {
                    id: rotationArea
                    expanded:  expandIndex == 3
                    onExpandedChanged: {
                        if(header.item){
                            header.item.active = expanded
                        }
                    }
                    header.sourceComponent: DDownArrowHeader {
                        text: dsTr("Rotation")
                        active: expandIndex == 3
                        onClicked: {
                            if(expandIndex == 3){
                                expandIndex = -1
                            }
                            else{
                                expandIndex = 3
                            }
                        }
                    }

                    content.sourceComponent:ListView {
                        id: rotationView
                        width: parent.width
                        height: childrenRect.height
                        spacing: 5

                        model: allMonitorsObjectsCount

                        delegate: RotationRectangle {
                            pOutputObj:allMonitorsObjects[index]
                            pOutputObjName: pOutputObj.name
                            shouldShowMonitorName: pOutputObj.opened && openedMonitors.length > 1
                            itemIndex: index
                        }
                    }

                }

                DSeparatorHorizontal {}

                DBaseExpand {
                    id:brightnessArea
                    visible: !singleBrightnessLine.visible
                    height: visible ? childrenRect.height : 0
                    expanded:  expandIndex == 4
                    onExpandedChanged: {
                        if(header.item){
                            header.item.active = expanded
                        }
                    }
                    header.sourceComponent: DDownArrowHeader {
                        text: dsTr("Brightness")
                        active: expandIndex == 4
                        onClicked: {
                            if(expandIndex == 4){
                                expandIndex = -1
                            }
                            else{
                                expandIndex = 4
                            }
                        }
                    }

                    content.sourceComponent:ListView {
                        id: brightnessView
                        width: parent.width
                        height: childrenRect.height

                        model: realMonitorsCount

                        delegate: BrightnessRectangle {
                            pOutputObjName: realMonitorsList[index]
                            itemIndex: index
                        }
                    }
                }

                DBaseLine {
                    id:singleBrightnessLine
                    visible: openedMonitors.length == 1 && !openedMonitors[0].isComposited
                    height: visible ? 30 : 0

                    leftLoader.sourceComponent: DssH2 {
                        text: dsTr("Brightness")
                    }

                    rightLoader.sourceComponent: DSliderEnhanced {
                        id: singleBrightnessSlider
                        width: 200
                        height: 28

                        min: 0
                        max: 1.0
                        init: typeof(openedMonitors[0]) != "undefined" ? brightnessValues[openedMonitors[0].name] : max
                        valueDisplayVisible: false

                        onValueChanged:{
                            if(openedMonitors[0] && value >= 0 && value <= 1 && pressedFlag){
                                displayId.SetBrightness(openedMonitors[0].name, value)
                            }
                        }
                    }
                }

                DSeparatorHorizontal {}
            }
        }
    }
}
