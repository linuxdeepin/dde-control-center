import QtQuick 2.0
import Deepin.Widgets 1.0

Rectangle {
    id: addLayoutItem
    width: parent.width
    height: 28
    color: dconstants.contentBgColor

    property string itemId: item_id
    property string layoutName: label

    property bool selected: false
    property bool hovered: false
    
    DssH3 {
        id: nameText
        anchors.left: parent.left
        anchors.leftMargin: 18
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 12
        text: layoutName
        width: parent.width - deleteButton.width - 40
        elide: Text.ElideRight
    }

    DssMultiAddCheckButton {
        id: deleteButton
        anchors.right: parent.right
        anchors.rightMargin: 18
        anchors.verticalCenter: parent.verticalCenter

        onClicked: {
            parent.selected = !parent.selected
        }
    }

    /***
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: parent.hovered = true
        onExited: parent.hovered = false
        onReleased: containsMouse ? parent.hovered = true : parent.hovered = false
        onClicked: {
            parent.selected = !parent.selected
        }
    }
    ***/
}

