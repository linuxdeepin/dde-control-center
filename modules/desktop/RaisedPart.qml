import QtQuick 2.1
import "../widgets"

Item {
    width: parent.width
    height: 30

    property int leftPadding: 20
    property int rightPadding: 20
    property string title: ""

    Text {
        color: "white"
        text: title
        font.pixelSize: 12

        anchors.left: parent.left
        anchors.leftMargin: parent.leftPadding
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -2
    }

    DSeparatorHorizontal {anchors.bottom: parent.bottom}
}
