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

    property var allRotations: outputObj.ListRotations()
    property var rotationModel: getRotationModel(allRotations)
    property var initExpanded: false

    property var rotationNames: {
        1: "Normal",
        2: "Rotate Left",
        4: "Upside Down",
        8: "Rotate Right",
    }

    function getResolutionModel(modes){
        var resolutionModel = listModelComponent.createObject(parent, {})
        for(var i=0; i<modes.length; i++){
            var resolution = getResolutionFromMode(modes[i])
            resolutionModel.append({
                "label": resolution,
                "selected": resolution == currentResolution,
                "modeId": modes[i][0]
            })
        }
        return resolutionModel
    }

    function getRotationModel(rotations){
        var rotation_model = listModelComponent.createObject(parent, {})
        for(var i=0; i<rotations.length; i++){
            var rotation = rotations[i]
            rotation_model.append({
                "label": rotationNames[rotation],
                "selected": rotation == currentRotation,
                "rotationId": rotations[i]
            })
        }
        return rotation_model
    }

    function in_array(e, a){
        for(var i=0; i<a.length; i++){
            if(a[i] == e){
                return true
            }
        }
        return false
    }

    function getResolutionFromMode(mode){
        return mode[1] + "x" + mode[2]
    }

    Column{
        width: parent.width
        height: childrenRect.height

        DSwitchButtonHeader {
            text: dsTr("Enabled")
            active: outputObj.opened
            onClicked: {
                outputObj.SwitchOn(active)
            }
            visible: monitorsNumber > 1
        }

        DSeparatorHorizontal {
            visible: monitorsNumber > 1
        }

        DBaseExpand {
            id: resolutionArea
            expanded: header.item.active
            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Resolution")
                hintText: " (" + currentResolution + ")"
                active: initExpanded
            }
        
            content.sourceComponent: DMultipleSelectView {
                id: modesView
                width: parent.width
                height: rows * 30

                columns: 3
                rows: Math.ceil(count/3)
                singleSelectionMode: true

                onSelect: {
                    outputObj.SetMode(resolutionModel.get(index).modeId)
                }

                Connections {
                    target: monitorProperties
                    onOutputObjChanged: {
                        if(outputObj){
                            var allResolutionModes = outputObj.ListModes()
                            modesView.model = getResolutionModel(allResolutionModes)
                        }
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
        
            content.sourceComponent: DMultipleSelectView {
                id: rotationsView
                width: parent.width
                height: rows * 30

                columns: 2
                rows: 2
                singleSelectionMode: true

                onSelect: {
                    outputObj.SetRotation(rotationModel.get(index).rotationId)
                }

                Connections {
                    target: monitorProperties
                    onOutputObjChanged: {
                        if(outputObj){
                            var rotations = outputObj.ListRotations()
                            rotationsView.model = getRotationModel(rotations)
                        }
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
                    displayId.SaveChanged()
                }
                visible: !outputObj.isComposited
            }
        }

        DSeparatorHorizontal{}
    }

}
