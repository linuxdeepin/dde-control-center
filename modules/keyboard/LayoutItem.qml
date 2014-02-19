import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    id: layoutItem
    width: parent.width
    height: 28
    anchors.left: parent.left
    anchors.leftMargin: 18

    property string itemId: item_id
    property string layoutName: label

    property bool selected: layoutItem.ListView.view.selectLayoutId == itemId
    property bool hovered: false
    property bool inDeleteAction: layoutItem.ListView.view.inDeleteAction

    Row {
        spacing: 5
        anchors.verticalCenter: parent.verticalCenter
        
        Image {
            id: nameImage
            width: deleteButton.width
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter
            source: "images/select.png"
            opacity: layoutItem.selected ? 1 : 0
        }

        DssMultiDeleteButton {
            id: deleteButton
            visible: {
                if(layoutItem.selected | !layoutItem.inDeleteAction){
                    nameImage.visible = true
                    return false
                }
                else {
                    nameImage.visible = false
                    return true
                }
            }

            onClicked: {
                layoutItem.ListView.view.deleteLayout(layoutItem.itemId)
            }
        }
        
        DssH3 {
            id: nameText
            anchors.verticalCenter: parent.verticalCenter
            text: layoutName
            color: {
                if(layoutItem.selected | layoutItem.hovered){
                    return "#009EFF"
                }
                else{
                    return "#FFF"
                }
            }
            font.pixelSize: 12
        }
    }
    
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        visible: !layoutItem.inDeleteAction
        
        onEntered: {
            layoutItem.hovered = true
        }

        onExited: {
            layoutItem.hovered = false
        }
        
        onClicked: {
            layoutItem.ListView.view.switchLayout(layoutItem.itemId)
        }
    }
}
