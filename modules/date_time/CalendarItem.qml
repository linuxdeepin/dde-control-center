import QtQuick 2.1

Rectangle {
    id: calendarItem
    width: 42
    height: 28
    color: "transparent"

    property alias inText: inText

    Text {
        id: inText
        anchors.centerIn: parent

        font.pixelSize: 12
        color: "white"
    }
}
