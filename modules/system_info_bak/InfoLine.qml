import QtQuick 2.0

Rectangle {
    id: container
    width: ListView.view.width
    height: 30
    color: "#252627"

    Text {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 22
        color: "#eaeaea"
        font.pixelSize: 13
        styleColor: "white"
        text: info
    }
}
