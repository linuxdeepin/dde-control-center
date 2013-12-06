import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Rectangle {
    id: header

    property string text: "untitled"
    property var dconstants: DConstants {}

    signal toggled

    width: parent.width
    height: 32 
    color: dconstants.bgColor

    ColumnLayout {
        width: parent.width
        anchors.verticalCenter: header.verticalCenter

        RowLayout {
            anchors.left:parent.left
            anchors.leftMargin: 10
            spacing: parent.width - label.width - arrow.width - anchors.leftMargin
            width: header.width

            Label {
                id:label
                text: header.text
                color: dconstants.fgColor
            }

            DStatusImageButton {
                id: arrow
                anchors.right:parent.right
                anchors.rightMargin: 10

                statusImageList: {
                    "down": ['images/arrow_down_normal.png', 'images/arrow_down_hover.png', 'images/arrow_down_press.png'],
                    "up": ['images/arrow_up_normal.png', 'images/arrow_up_hover.png', 'images/arrow_up_press.png']
                }
                currentStatus: "down"

                onClicked: {
                    toggled()
                    currentStatus = currentStatus == "down" ? "up" : "down"
                }
            }
        }
    }
}
