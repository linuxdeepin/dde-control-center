import QtQuick 2.1
import Deepin.Widgets 1.0

Item {
    id: wrapper
    width: wrapper.ListView.view.width; height: 26

    property alias text: label.text

    Rectangle {
        color: itemOnHover ? "#141414" : "#191919"
        anchors.fill: parent
    }

    Image {
        id: headImg
        source: itemOnHover ? "../images/select-dark-hover.png" : "../images/select-dark.png"
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 5
        visible: index == 0
    }

    DLabel {
        id: label
        anchors.left: parent.left
        anchors.leftMargin: 28
        anchors.verticalCenter: parent.verticalCenter
        text: "text " + index
        font.pixelSize: 12
        color: itemOnHover ? DConstants.activeColor : DConstants.fgColor
    }
}
