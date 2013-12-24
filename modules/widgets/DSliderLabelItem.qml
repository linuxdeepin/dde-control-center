import QtQuick 2.1
import QtQuick.Controls 1.0
import "../widgets/"

Item {
    width: label.width
    height: label.height
    property int edgeMargin: 0

    property alias text: label.text

    Rectangle {
        anchors.top: parent.top
        anchors.topMargin: -1 * height
        anchors.left: parent.left
        anchors.leftMargin: edgeMargin
        color: "white"
        width: 1
        height: 6
    }

    DssH3 {
        id: label
        anchors.centerIn: parent
    }
}
