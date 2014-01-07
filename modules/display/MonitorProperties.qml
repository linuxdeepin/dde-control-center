import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

Item {
    width: parent.width
    height: childrenRect.height
    property var outputObj

    property var resolutionModel: getResolutionModel(outputObj.ListModes())
    property string currentResolution: getResolutionFromMode(outputObj.mode)

    Component {
        id: list_model
        ListModel {}
    }

    function getResolutionModel(modes){
        var resolutionModel = list_model.createObject(parent, {})
        var tempArray = new Array()
        for(var i=0; i<modes.length; i++){
            var resolution = getResolutionFromMode(modes[i])
            if(!in_array(resolution, tempArray)){
                tempArray.push(resolution)
                resolutionModel.append({
                    "label": resolution,
                    "selected": resolution == currentResolution
                })
            }
        }
        return resolutionModel
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
        return mode[0] + "x" + mode[1]
    }

    DBaseExpand {
        id: resolutionArea
        header.sourceComponent: DDownArrowHeader {
            text: dsTr("Resolution")
            onClicked: resolutionArea.expanded = active
        }
    
        content.sourceComponent: DMultipleSelectView {
            id: hotspot_top_left_select_view
            width: parent.width
            height: rows * 30

            columns: 3
            rows: Math.ceil(resolutionModel.count/3)
            singleSelectionMode: true

            model: resolutionModel
            onSelect: {
                print(index)
            }
        }
    }
}
