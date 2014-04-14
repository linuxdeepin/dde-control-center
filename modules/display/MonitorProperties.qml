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

    property var monitorNames: Object.keys(outputObj.brightness)
    property var brightnessValues: outputObj.brightness

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
                            for(var i=0; i<modes.length; i++){
                                var resolution = getResolutionFromMode(modes[i])
                                if(!valueDict[resolution]){
                                    valueDict[resolution] = []
                                }
                                valueDict[resolution].push(modes[i][0])
                            }

                            modesView.model.clear()
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
                        text: "Brightness"
                    }

                    rightLoader.sourceComponent: DSliderEnhanced {
                        id: oneBrightnessSlider
                        width: sliderWidth
                        height: 28
                        min: 0
                        max: 1.0
                        init: 0
                        valueDisplayVisible: false

                        onValueChanged:{
                            outputObj.SetBrightness(monitorNames[0], value)
                        }
                        visible: monitorNames.length == 1

                        Connections {
                            target: monitorProperties
                            onBrightnessValuesChanged: {
                                if (!oneBrightnessSlider.pressedFlag) {
                                    oneBrightnessSlider.setValue(brightnessValues[monitorNames[0]])
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
                            width: sliderWidth
                            height: 28
                            min: 0
                            max: 1.0
                            init: 0
                            valueDisplayVisible: false

                            onValueChanged:{
                                outputObj.SetBrightness(monitorNames[index], value)
                            }

                            Connections {
                                target: monitorProperties
                                onBrightnessValuesChanged: {
                                    if (!multiBrightnessSlider.pressedFlag) {
                                        multiBrightnessSlider.setValue(brightnessValues[monitorNames[index]])
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
