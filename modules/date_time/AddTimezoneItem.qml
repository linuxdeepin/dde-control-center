import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    width: parent.width
    height: 28
    anchors.left: parent.left
    anchors.leftMargin: 15
    anchors.right: parent.right
    anchors.rightMargin: 15

    property string itemValue: value
    property string itemLabel: label

    property bool selected: false

    signal selectAction(string itemValue, bool selected)

    Component.onCompleted: {
    }

    DssH3 {
        id: nameText
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        text: itemLabel
        color: selected ? "#009EFF" : "#fff"
        font.pixelSize: 12
    }

    DssMultiAddCheckButton {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        onClicked: {
            parent.selected = !parent.selected
            selectAction(itemValue, selected)
        }
    }
}
