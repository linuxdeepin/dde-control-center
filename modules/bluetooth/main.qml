import QtQuick 2.1

Rectangle {
    anchors.fill: parent
    color: childColor

    Text {
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 20
        font.pixelSize: 14
        color: 'white'
        text: iconId
    }
}
