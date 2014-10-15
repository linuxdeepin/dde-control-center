import QtQuick 2.1
import Deepin.Widgets 1.0

Rectangle {
    id: title_separator
    width: 100
    height: 30
    color: "transparent"

    property int leftMargin: 15
    property string text: ""

    DssH2 {
        id: title
        text: title_separator.text
        visible: parent.height <= 0 ? false : true
        anchors.left: parent.left
        anchors.leftMargin: title_separator.leftMargin
        anchors.verticalCenter: parent.verticalCenter
    }
}
