import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    width: parent.width
    height: 28
    anchors.left: parent.left
    anchors.leftMargin: 15
    property bool hovered: false

    property string itemValue: value
    property string itemLabel: label

    property string selectedItemValue: ""
    property bool inDeleteAction: false

    signal deleteAction(string itemValue)
    signal selectAction(string itemValue)

    Row {
        spacing: 5
        anchors.verticalCenter: parent.verticalCenter
        
        Image {
            id: nameImage
            width: deleteButton.width
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter
            source: "images/select.png"
            opacity: selectedItemValue == itemValue
        }

        DssMultiDeleteButton {
            id: deleteButton
            opacity: visible ? 1 : 0
            visible: {
                if(selectedItemValue == itemValue | !inDeleteAction){
                    nameImage.visible = true
                    return false
                }
                else {
                    nameImage.visible = false
                    return true
                }
            }

            onClicked: {
                deleteAction(itemValue)
            }

            Behavior on opacity {
                PropertyAnimation { duration: 150 }
            }
        }
        
        DssH3 {
            id: nameText
            anchors.verticalCenter: parent.verticalCenter
            text: itemLabel
            color: {
                if(selectedItemValue == itemValue | hovered){
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
        visible: !inDeleteAction
        
        onEntered: hovered = true
        onExited: hovered = false
        onClicked: {
            if(selectedItemValue != itemValue) {
                selectAction(itemValue)
            }
        }
    }
}
