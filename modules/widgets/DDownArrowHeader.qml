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
            Image {
                anchors.right:parent.right
                anchors.rightMargin: 10
                id: arrow
                source: "images/arrow_down.png"
                MouseArea {
                    anchors.fill:parent
                    onClicked: {
                        toggled()
                    }
                }
            }
        }
    }
}
