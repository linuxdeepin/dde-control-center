import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Rectangle {
    id: header

    property string text: dsTr("Untitled")
    property string darkText
    property alias active: actionArea.checked

    property int leftMargin: 18
    property int rightMargin: 5
    property var dconstants: DConstants {}

    height: 30
    width: parent.width
    color: dconstants.bgColor

    signal clicked

    DssH2 {
        id: titleArea
        anchors.left: parent.left
        anchors.leftMargin: leftMargin
        anchors.verticalCenter: parent.verticalCenter
        text: header.text
    }

    DssH2 {
        id: darkArea
        anchors.left: titleArea.right
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 12
        color: dconstants.fgDarkColor
        visible: darkText ? true : false
        text: " (" + header.darkText + ")"
    }

    Item {
        anchors.right: parent.right
        anchors.rightMargin: rightMargin
        anchors.verticalCenter: parent.verticalCenter
        height: parent.height - 2
        width: actionArea.width

        DDownArrowButton {
            id: actionArea
            anchors.centerIn: parent

            onClicked: {
                header.clicked()
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            actionArea.checked = !actionArea.checked
            actionArea.clicked()
        }
    }
}
