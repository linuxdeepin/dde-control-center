import QtQuick 2.1

Rectangle {
    anchors.top: parent.top
    width: 2
    height: parent.height

    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        width: 1
        height: parent.height
        color: "#120f10"
    }
    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 1
        width: 1
        height: parent.height
        color: "#303132"
    }
}
