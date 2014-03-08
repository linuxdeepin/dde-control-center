import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import Deepin.Widgets 1.0

Item {
    id: monitorProperties
    width: parent.width
    height: childrenRect.height
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

            DSwitchButtonHeader {
                id: enabledSwitcher
                text: dsTr("Enabled")
                active: outputObj.opened
                onClicked: {
                    outputObj.SwitchOn(!outputObj.opened)
                }
            }

            DSeparatorHorizontal{}
        }

        Item {
            width: parent.width
            height: enabledSwitcher.active ? propertyColumn.height : 0
            clip: true

            Behavior on height{
                PropertyAnimation { duration: 150 }
            }

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

                        model: {
                            var resolutionModel = listModelComponent.createObject(modesView, {})
                            var modes = outputObj.ListModes()
                            for(var i=0; i<modes.length; i++){
                                var resolution = getResolutionFromMode(modes[i])
                                resolutionModel.append({
                                    "item_label": resolution,
                                    "item_value": modes[i][0]
                                })
                            }
                            return resolutionModel
                        }

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

                    rightLoader.sourceComponent: DSlider{
                        value: outputObj.isComposited ? 0 : outputObj.brightness[outputObj.name]
                        onValueChanged: {
                            outputObj.ChangeBrightness(outputObj.name, value)
                        }
                        visible: !outputObj.isComposited
                    }
                }
                DSeparatorHorizontal {}
            }
        }
    }
}
