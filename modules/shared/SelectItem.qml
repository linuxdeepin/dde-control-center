import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    id: selectItem
    width: parent.width
    height: 28
    anchors.left: parent.left
    anchors.leftMargin: 18
    property int labelLeftMargin: 18
    property bool selected: selectItemId == itemId
    property bool hovered: false

    // info from model
    property string itemId: item_id
    property string itemName: item_name
    // info from model

    // Need binding variable
    property int totalItemNumber: 0
    property string selectItemId: ""
    property bool inDeleteAction: false
    // Need binding variable

    signal deleteAction(string itemId)
    signal selectAction(string itemId)

    Image {
        id: nameImage
        width: deleteButton.width
        fillMode: Image.PreserveAspectFit
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: nameText.left
        source: "images/select.png"
    }

    DssMultiDeleteButton {
        id: deleteButton
        anchors.right: nameText.left
        anchors.verticalCenter: parent.verticalCenter
        opacity: {
            if(selected){
                nameImage.visible = (totalItemNumber != 1)
                return 0
            }
            else{
                nameImage.visible = false
                if(inDeleteAction){
                    return 1
                }
                else{
                    return 0
                }
            }
        }

        visible: opacity != 0

        onClicked: {
            deleteAction(itemId)
        }

        Behavior on opacity {
            PropertyAnimation {duration: 150}
        }
    }
    
    DssH3 {
        id: nameText
        width: parent.width - nameImage.width - deleteButton.width
        anchors.left: parent.left
        anchors.leftMargin: labelLeftMargin
        anchors.verticalCenter: parent.verticalCenter
        text: itemName
        elide: Text.ElideRight
        color: {
            if(selected){
                return dconstants.activeColor
            }else if(hovered){
                return dconstants.hoverColor
            }
            else{
                return dconstants.fgColor
            }
        }
        font.pixelSize: 12
    }
    
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        visible: !inDeleteAction
        
        onEntered: {
            toolTip.showTip(itemName)
            hovered = true
        }

        onExited: {
            toolTip.hideTip()
            hovered = false
        }
        
        onClicked: {
            selectAction(itemId)
        }
    }

    ListView.onAdd: SequentialAnimation {
        PropertyAction { target: selectItem; property: "height"; value: 0 }
        NumberAnimation { target: selectItem; property: "height"; to: 28; duration: 250; easing.type: Easing.InOutQuad }
    }

    ListView.onRemove: SequentialAnimation {
        PropertyAction { target: selectItem; property: "ListView.delayRemove"; value: true }
        NumberAnimation { target: selectItem; property: "height"; to: 0; duration: 250; easing.type: Easing.InOutQuad }

        // Make sure delayRemove is set back to false so that the item can be destroyed
        PropertyAction { target: selectItem; property: "ListView.delayRemove"; value: false }
    }

}
