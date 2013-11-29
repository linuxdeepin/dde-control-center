import QtQuick 2.1

Rectangle {
    width: parent.width
    height: 28
    color: "transparent"

    property string text
    property alias button: button

    signal clicked

    Text {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 18
        font.pixelSize: 13
        color: textColor
        text: parent.text
    }

    DSwitcherButton {
        id: button
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right

        onClicked: {
            parent.clicked()
        }
    }
}
