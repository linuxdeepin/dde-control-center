import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    width: parent.width
    height: 28
    anchors.left: parent.left
    anchors.leftMargin: 25

    property string itemId: item_id
    property string layoutName: label
    
    Row {
        spacing: 5
        anchors.verticalCenter: parent.verticalCenter
        
        Image {
            id: nameImage
            anchors.verticalCenter: parent.verticalCenter
            source: "images/select.png"
            opacity: keyboardLayoutSetting.defaultSelectItemId == itemId ? 1 : 0
        }
        
        DssH3 {
            id: nameText
            anchors.verticalCenter: parent.verticalCenter
            text: layoutName
            color: keyboardLayoutSetting.defaultSelectItemId == itemId ? "#009EFF" : "#fff"
            font.pixelSize: 12
        }
    }
    
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        
        onEntered: {
            layoutList.currentIndex = index
        }
        
        onClicked: {
            keyboardID.keyboardLayout = [itemId]
        }
    }
}
