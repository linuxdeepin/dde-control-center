import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

Item {
    width: parent.width
    height: childrenRect.height
    property var outputObj

    property string currentResolution: getResolutionFromMode(outputObj.mode)
    property int currentRotation: outputObj.rotation

    property var allResolutionModes: outputObj.ListModes()
    property var allRotations: outputObj.ListRotations()
    property var resolutionModel: getResolutionModel(allResolutionModes)
    property var rotationModel: getRotationModel(allRotations)

    property var rotationNames: {
        1: "Normal",
        2: "Rotate Left",
        4: "Upside Down",
        8: "Rotate Right",
    }

    property var reflectNames: {
        0: "Normal",
        16: "x reflect",
        32: "y reflect",
        48: "x,y reflect"
    }

    Component {
        id: list_model
        ListModel {}
    }

    function getResolutionModel(modes){
        var resolutionModel = list_model.createObject(parent, {})
        for(var i=0; i<modes.length; i++){
            var resolution = getResolutionFromMode(modes[i])
            resolutionModel.append({
                "label": resolution,
                "selected": resolution == currentResolution
            })
        }
        return resolutionModel
    }

    function getRotationModel(rotations){
        var rotation_model = list_model.createObject(parent, {})
        for(var i=0; i<rotations.length; i++){
            var rotation = rotations[i]
            rotation_model.append({
                "label": rotationNames[rotation],
                "selected": rotation == currentRotation
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
        return mode[1] + "x" + mode[2] + "x" + mode[3]
    }

    Column{
        width: parent.width
        height: childrenRect.height

        DBaseExpand {
            id: outputSwitch
            expanded: header.item.active
            header.sourceComponent: DSwitchButtonHeader {
                text: outputObj.name
                active: outputObj.opened
                onClicked: {
                    //outputObj.opened = active
                }
            }

            content.sourceComponent: Component{
                Column{
                    width: parent.width
                    height: resolutionArea.height + rotationArea.height

                    DBaseExpand {
                        id: resolutionArea
                        header.sourceComponent: DDownArrowHeader {
                            text: dsTr("Resolution")
                            hintText: " (" + currentResolution + ")"
                            leftMargin: 25
                            onClicked: resolutionArea.expanded = active
                        }
                    
                        content.sourceComponent: DMultipleSelectView {
                            width: parent.width
                            height: rows * 30

                            columns: 3
                            rows: Math.ceil(resolutionModel.count/3)
                            singleSelectionMode: true

                            model: resolutionModel
                            onSelect: {
                                outputObj.SetMode(allResolutionModes[index][0])
                            }
                        }
                    }

                    DSeparatorHorizontal {}

                    DBaseExpand {
                        id: rotationArea
                        header.sourceComponent: DDownArrowHeader {
                            text: dsTr("Rotation")
                            hintText: rotationNames[currentRotation]
                            leftMargin: 25
                            onClicked: rotationArea.expanded = active
                        }
                    
                        content.sourceComponent: DMultipleSelectView {
                            width: parent.width
                            height: rows * 30

                            columns: 2
                            rows: 2
                            singleSelectionMode: true

                            model: rotationModel
                            onSelect: {
                                outputObj.rotation = allRotations[index]
                            }
                        }
                    }

                    DSeparatorHorizontal {}

                } //end Column
            } //end Component
        } //end content.sourceComponent

        DSeparatorHorizontal {}
    } // end Column
}
