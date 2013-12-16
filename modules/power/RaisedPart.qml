import QtQuick 2.1

Rectangle {
    width: 310
    height: 30

    color: "#252627"

    property int leftPadding: 20
    property int rightPadding: 20
    property string title: ""

    Rectangle {
        id: top_shade

        width: parent.width
        height: 1

        color: Qt.rgba(1, 1, 1, 0.08)
        anchors.top: parent.top
    }

    Text {
        color: "white"
        text: parent.title
        font.pixelSize: 12

        anchors.left: parent.left
        anchors.leftMargin: parent.leftPadding
        anchors.verticalCenter: parent.verticalCenter
    }

    Rectangle {
        id: bottom_shade

        width: parent.width
        height: 1

        color: Qt.rgba(0, 0, 0, 0.5)
        anchors.bottom: parent.bottom
    }
}