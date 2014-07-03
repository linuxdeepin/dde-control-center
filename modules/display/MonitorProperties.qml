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

Item {
    id: monitorProperties
    width: parent.width
    height: childrenRect.height

    property int sliderWidth: 186
    property var listModelComponent: DListModelComponent {}

    property var outputObj
    property int monitorsNumber: 0

    property string currentResolution: getResolutionFromMode(outputObj.currentMode)
    property int currentRotation: outputObj.rotation

    property var initExpanded: true

    property var rotationNames: {
        1: dsTr("Normal"),
        2: dsTr("Rotate Right"),
        4: dsTr("Upside Down"),
        8: dsTr("Rotate Left"),
    }

    property var monitorNames: outputObj.outputs
    property var brightnessValues: displayId.brightness

    onOutputObjChanged: {
        if(outputObj){
            enabledSwitcher.rightLoader.item.checked = outputObj.opened
            resolutionArea.content.item.loadResolutionModel()
        }
    }

    function getResolutionFromMode(mode){
        return mode[1] + "x" + mode[2]
    }

    Column {
        id: enabledColumn
        width: parent.width
        height: childrenRect.height

        Column{
            width: parent.width
            height: childrenRect.height
            visible: monitorsNumber > 1

            DBaseLine{
                id: enabledSwitcher
                leftLoader.sourceComponent: DssH2{
                    text: dsTr("Enabled")
                }
                rightLoader.sourceComponent: DSwitchButton{
                    onClicked: {
                        outputObj.SwitchOn(!outputObj.opened)
                    }
                }
            }


            DSeparatorHorizontal{}
        }

        Item {
            width: parent.width
            height: outputObj.opened ? propertyColumn.height : 0
            clip: true

            //Behavior on height{
                //PropertyAnimation { duration: 150 }
            //}

            Column{
                id: propertyColumn
                clip: true
                width: parent.width
                height: childrenRect.height

                DBaseExpand {
                    id: resolutionArea
                    expanded: header.item.active
                    header.sourceComponent: DDownArrowHeader {
                        text: dsTr("Resolution")
                        hintText: " (" + currentResolution + ")"
                        active: initExpanded
                    }
                
                    content.sourceComponent: GridView{
                        id: modesView
                        width: parent.width
                        height: Math.ceil(count/3) * 30

                        cellWidth: width/3
                        cellHeight: 30
                        property int currentValue: outputObj.currentMode[0]
                        property var valueDict: new Object()

                        function loadResolutionModel(){
                            var modes = outputObj.ListModes()
                            modesView.model.clear()
                            valueDict = new Object()
                            for(var i=0; i<modes.length; i++){
                                var resolution = getResolutionFromMode(modes[i])
                                if(!valueDict[resolution]){
                                    valueDict[resolution] = []
                                }
                                valueDict[resolution].push(modes[i][0])
                            }

                            for(var key in valueDict){
                                modesView.model.append({
                                    "item_label": key,
                                    "item_value": valueDict[key][0]
                                })
                            }
                        }

                        model: ListModel{}

                        delegate: ModeItem {
                            modesDict: modesView.valueDict
                            currentValue: modesView.currentValue
                            onSelectAction: {
                                outputObj.SetMode(itemValue)
                            }
                        }
                    }
                }

                DSeparatorHorizontal {}

                DBaseExpand {
                    id: rotationArea
                    expanded: header.item.active
                    header.sourceComponent: DDownArrowHeader {
                        text: dsTr("Rotation")
                        hintText: " (" + rotationNames[currentRotation] + ")"
                        active: initExpanded
                    }
                
                    content.sourceComponent: GridView{
                        id: rotationView
                        width: parent.width
                        height: Math.ceil(count/2) * 30

                        cellWidth: width/2
                        cellHeight: 30
                        property int currentValue: outputObj.rotation

                        model: {
                            var rotation_model = listModelComponent.createObject(rotationView, {})
                            var rotations = outputObj.ListRotations()
                            for(var i=0; i<rotations.length; i++){
                                var rotation = rotations[i]
                                rotation_model.append({
                                    "item_label": rotationNames[rotation],
                                    "item_value": rotations[i]
                                })
                            }
                            return rotation_model
                        }

                        delegate: RotationItem {
                            currentValue: rotationView.currentValue
                            onSelectAction: {
                                outputObj.SetRotation(itemValue)
                            }
                        }
                    }

                }

                DSeparatorHorizontal {}

                DBaseLine {
                    leftLoader.sourceComponent: DssH2 {
                        text: dsTr("Brightness")
                    }

                    rightLoader.sourceComponent: DSliderEnhanced {
                        id: oneBrightnessSlider
                        property string outputName: monitorNames ? monitorNames[0] : ""
                        width: sliderWidth
                        height: 28
                        min: 0
                        max: 1.0
                        init: outputName ? brightnessValues[outputName] : max
                        valueDisplayVisible: false

                        onValueChanged:{
                            if(outputName && value >= 0 && value <= 1){
                                displayId.SetBrightness(outputName, value)
                            }
                        }
                        visible: monitorNames.length == 1

                        onOutputNameChanged: {
                            if (outputName && brightnessValues)
                                setValue(brightnessValues[outputName])
                        }

                        Connections {
                            target: monitorProperties
                            onBrightnessValuesChanged: {
                                if(!oneBrightnessSlider.pressedFlag && oneBrightnessSlider.outputName != "") {
                                    oneBrightnessSlider.setValue(brightnessValues[oneBrightnessSlider.outputName])
                                }
                            }
                        }
                    }
                }

                DSeparatorHorizontal {}

                Repeater{
                    model: monitorNames
                    delegate: DBaseLine{
                        width: monitorProperties.width
                        visible: monitorNames.length > 1
                        leftMargin: 18

                        leftLoader.sourceComponent: LeftTitle{
                            width: 70
                            text: monitorNames[index]
                        }

                        rightLoader.sourceComponent: DSliderEnhanced {
                            id: multiBrightnessSlider
                            property var outputName: monitorNames[index]
                            width: sliderWidth
                            height: 28
                            min: 0
                            max: 1.0
                            init: brightnessValues[outputName]
                            valueDisplayVisible: false

                            onValueChanged:{
                                if(outputName && value >= 0 && value <= 1){
                                    displayId.SetBrightness(outputName, value)
                                }
                            }

                            onOutputNameChanged:{
                                setValue(brightnessValues[outputName])
                            }

                            Connections {
                                target: monitorProperties
                                onBrightnessValuesChanged: {
                                    if (!multiBrightnessSlider.pressedFlag) {
                                        multiBrightnessSlider.setValue(brightnessValues[multiBrightnessSlider.outputName])
                                    }
                                }
                            }
                        }
                    }
                }

                DSeparatorHorizontal { visible: outputObj.isComposited }
            }
        }
    }
}
