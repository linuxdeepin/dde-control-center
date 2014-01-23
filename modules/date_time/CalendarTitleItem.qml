import QtQuick 2.1

Rectangle {
    id: calendarItem
    width: 44
    height: 30
    color: "#1a1b1b"
    //color: "transparent"

    property alias inText: inText
    property bool highlight: isHighlight
    property color highlightColor: "#009eff"

    // start border
    Rectangle{
        // left
        anchors.top: parent.top
        anchors.left: parent.left
        width: 1
        height: parent.height
        color: "#303132"
    }
    Rectangle{
        // top
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width
        height: 1
        color: "#303132"
    }
    Rectangle{
        // right
        anchors.top: parent.top
        anchors.right: parent.right
        width: 1
        height: parent.height
        color: "#120f10"
    }
    Rectangle{
        // bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: parent.width
        height: 1
        color: "#120f10"
    }
    // end border

    Text {
        id: inText
        anchors.centerIn: parent

        font.pixelSize: 12
        color: highlight ? highlightColor : dconstants.fgColor
        text: dayText
    }

}
