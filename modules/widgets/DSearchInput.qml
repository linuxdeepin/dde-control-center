import QtQuick 2.1

DTextInput {
    textInputRightMargin: search_image.width + searchButtonRightMargin
    
    property int searchButtonRightMargin: 6
    
    DImageButton {
        id: search_image
        
        normal_image: "images/search_normal.png"
        hover_image: "images/search_hover.png"
        press_image: "images/search_hover.png"
        
        anchors.right: parent.right
        anchors.rightMargin: parent.searchButtonRightMargin
        anchors.verticalCenter: parent.verticalCenter
    }
}