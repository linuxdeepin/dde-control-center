import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Rectangle {
    id: header

    property string text: dsTr("Untitled")
    property string hintText
    property alias icon: iconArea.source
    property alias active: actionArea.checked

    property int leftMargin: 15
    property int rightMargin: 5
    property var dconstants: DConstants {}

    height: 30
    width: parent.width
    color: dconstants.bgColor

    signal clicked

    Item {
        anchors.fill: parent
        anchors.left: parent.left
        anchors.leftMargin: leftMargin

        Row {
            height: parent.height
            width: parent.width

            Item {
                width: iconArea.width + 8
                height: iconArea.height
                anchors.verticalCenter: parent.verticalCenter
                Image {
                    id: iconArea
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    visible: source ? true : false
                }
            }

            DssH2 {
                id: titleArea
                text: header.text
                anchors.verticalCenter: parent.verticalCenter
            }

            DssH2 {
                id: darkArea
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 12
                color: dconstants.fgDarkColor
                visible: hintText ? true : false
                text: " (" + header.hintText + ")"
            }
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
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            actionArea.checked = !actionArea.checked
            actionArea.clicked()
        }
    }
}
