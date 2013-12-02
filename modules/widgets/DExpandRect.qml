import QtQuick 2.1

Rectangle {

    property alias head: headText
    property bool expand: false

    width: parent.width
    height: 28
    color: bgColor

    Text {
        id: headText
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 18
        font.pixelSize: 13
        color: fgColor
    }

    DImageCheckButton {
        color: bgColor
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 12

        inactivatedNomralImage: "images/arrow_down_normal.png"
        inactivatedHoverImage: "images/arrow_down_hover.png"
        inactivatedPressImage: "images/arrow_down_press.png"

        activatedNomralImage: "images/arrow_up_normal.png"
        activatedHoverImage: "images/arrow_up_hover.png"
        activatedPressImage: "images/arrow_up_press.png"

        onActivateChanged: {
            expand = activate
        }
    }
}
