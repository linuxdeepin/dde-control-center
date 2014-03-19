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
        1: "Normal",
        2: "Rotate Left",
        4: "Upside Down",
        8: "Rotate Right",
    }

    onOutputObjChanged: {
        if(outputObj){
            enabledSwitcher.rightLoader.item.checked = outputObj.opened
        }
    }

    function getResolutionFromMode(mode){
        return mode[1] + "x" + mode[2]
    }

    Column{
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

                        function getResolutionModel(){
                            var resolutionModel = listModelComponent.createObject(modesView, {})
                            var modes = outputObj.ListModes()
                            var checkRepeatDict = {}
                            resolution = getResolutionFromMode(outputObj.bestMode)
                            resolutionModel.append({
                                "item_label": resolution + " *",
                                "item_value": outputObj.bestMode[0]
                            })
                            checkRepeatDict[resolution] = true
                            for(var i=0; i<modes.length; i++){
                                var resolution = getResolutionFromMode(modes[i])
                                if(!checkRepeatDict[resolution]){
                                    resolutionModel.append({
                                        "item_label": resolution,
                                        "item_value": modes[i][0]
                                    })
                                    checkRepeatDict[resolution] = true
                                }
                            }
                            return resolutionModel
                        }

                        model: getResolutionModel()

                        delegate: PropertyItem {
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

                        delegate: PropertyItem {
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
                        width: sliderWidth
                        height: 28
                        min: 0
                        max: 1.0
                        init: outputObj.isComposited ? 0 : outputObj.brightness[outputObj.name]
                        valueDisplayVisible: false

                        onValueConfirmed:{
                            outputObj.ChangeBrightness(outputObj.name, value)
                        }
                        visible: !outputObj.isComposited
                    }
                }
                DSeparatorHorizontal {}

                Repeater{
                    model: Object.keys(outputObj.brightness)
                    delegate: DBaseLine{
                        width: monitorProperties.width
                        visible: outputObj.isComposited
                        leftMargin: 18

                        leftLoader.sourceComponent: LeftTitle{
                            width: 70
                            text: modelData
                        }

                        rightLoader.sourceComponent: DSliderEnhanced {
                            width: sliderWidth
                            height: 28
                            min: 0
                            max: 1.0
                            init: outputObj.brightness[modelData]
                            valueDisplayVisible: false

                            onValueConfirmed:{
                                outputObj.ChangeBrightness(modelData, value)
                            }
                        }
                    }
                }

                DSeparatorHorizontal { visible: outputObj.isComposited }
            }
        }
    }
}
