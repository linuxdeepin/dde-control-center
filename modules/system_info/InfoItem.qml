import QtQuick 2.1

Rectangle {
    width: parent.width
    height: 28
    color: "#1a1b1b"

    property string title
    property string value
    property int fontSize: 13
    property int leftWidth: 100

    Rectangle {
        id: itemTitleBox
        anchors.top: parent.top
        anchors.left: parent.left
        width: leftWidth
        height: parent.height
        color: "transparent"

        Text {
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.verticalCenter: parent.verticalCenter
            color: textColor
            font.pixelSize: fontSize
            text: title
        }
    }

    Text {
        id: systemVersion
        anchors.left: itemTitleBox.right
        anchors.leftMargin: 5
        anchors.verticalCenter: itemTitleBox.verticalCenter
        color: textColor
        font.pixelSize: fontSize
        text: value
    }
}
