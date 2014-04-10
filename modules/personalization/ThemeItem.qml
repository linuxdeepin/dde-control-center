import QtQuick 2.1
import Deepin.Widgets 1.0

Item {
    id: themeItem

    property string selectedItemValue: ""
    property string itemValue: item_value

    property bool hovered: false
    property bool selected: selectedItemValue == itemValue

    signal selectAction(string itemValue)
    signal previewAction(string itemValue)

    Rectangle{
        id: selectedBackground
        width: parent.width - 6
        height: parent.height - 6
        anchors.centerIn: parent
        color: "#101010"
        radius: 3
        visible: parent.selected
    }
    
    Rectangle {
        id: itemThumbnailBox
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 8
        width: 130
        height: 74
        color: "transparent"
        border.width: 1
        border.color: {
            if(parent.selected){
                return dconstants.activeColor
            }
            else if(parent.hovered){
                return dconstants.hoverColor
            }
            else{
                return dconstants.fgColor
            }
        }

        Image {
            id: itemThumbnailImage
            anchors.centerIn: parent
            source: item_img_url
            width: parent.width - 2
            height: parent.height - 2
        }
    }

    DssH2 {
        id: itemNameArea
        anchors.top: itemThumbnailBox.bottom
        anchors.topMargin: 6
        anchors.horizontalCenter: parent.horizontalCenter
        text: item_name
        color: {
            if(parent.selected){
                return dconstants.activeColor
            }
            else if(hovered){
                return dconstants.hoverColor
            }
            else{
                return dconstants.fgColor
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onClicked: {
            selectAction(itemValue)
        }

        onEntered: {
            parent.hovered = true
            previewAction(themeObj)
        }

        onExited: {
            parent.hovered = false
        }
    }

    /***
    Rectangle{
        id: zoomButtonBox
        anchors.top: itemThumbnailBox.top
        anchors.topMargin: 3
        anchors.right: itemThumbnailBox.right
        anchors.rightMargin: 3
        width: zoomButton.width + 16
        height: zoomButton.height + 4
        radius: height/2
        color: Qt.rgba(1, 1, 1, 0.5)
        border.color: Qt.rgba(0, 0, 0, 0.5)
        border.width: 1

        DOpacityImageButton{
            id: zoomButton
            source: "images/zoom.gif"
            anchors.centerIn: parent
            onClicked: previewAction(itemValue)
        }
    }
    ***/
}
