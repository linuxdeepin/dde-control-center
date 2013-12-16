import QtQuick 2.1

Rectangle {
    width: 310
    height: 30 * rows

    property int rows: 1
    property int topPadding: 10
    property int bottonPadding: 10
    property int leftPadding: 15
    property int rightPadding: 15

    color: "#191919"

    Rectangle {
        id: top_shade

        width: parent.width
        height: 1

        color: Qt.rgba(0, 0, 0, 0.1)
        anchors.top: parent.top
    }
    Rectangle {
        id: bottom_shade

        width: parent.width
        height: 1

        color: Qt.rgba(0, 0, 0, 0.3)
        anchors.bottom: parent.bottom
    }
}