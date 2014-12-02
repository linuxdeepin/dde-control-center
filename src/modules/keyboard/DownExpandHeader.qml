import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0

Rectangle {
    id: header

    property string text: "Untitled"
    property string hintTextId
    property alias icon: iconImage.source
    property alias active: actionButton.checked

    property var dconstants: DConstants {}
    property int leftMargin: dconstants.leftMargin
    property int rightMargin: dconstants.rightMargin
    focus: true

    height: 30
    width: parent.width
    color: dconstants.bgColor

    signal clicked

    Item {
        anchors.fill: parent
        anchors.left: parent.left
        anchors.leftMargin: leftMargin
        anchors.right: parent.right
        anchors.rightMargin: rightMargin

        Row {
            height: parent.height
            width: parent.width - actionArea.width
            spacing: 2

            Item {
                id: iconArea
                width: iconImage.width + 8
                height: iconImage.height
                anchors.verticalCenter: parent.verticalCenter
                Image {
                    id: iconImage
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    visible: source ? true : false
                }
            }

            DssH2 {
                id: titleArea
                text: header.text + "  "
                anchors.verticalCenter: parent.verticalCenter
            }

            Image {
                anchors.verticalCenter: parent.verticalCenter
                visible: hintTextId == undefined ? false : true
                source: hintTextId == undefined ? "" : "lang_images/dark/" + hintTextId + ".png"
            }
        }

        Item {
            id: actionArea
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height - 2
            width: actionButton.width

            DDownArrowButton {
                id: actionButton
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
            actionButton.checked = !actionButton.checked
            actionButton.clicked()
        }
    }
}
