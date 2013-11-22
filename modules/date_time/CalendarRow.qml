import QtQuick 2.1

Rectangle {
    anchors.horizontalCenter: parent.horizontalCenter
    width: 310
    height: 28
    color: "#1a1b1b"

    Row {
        property variant sevenString: ["日", "一", "二", "三", "四", "五", "六"]

        CalendarItem { inText.text: parent.sevenString[0]}
        SepratorVertical {}

        CalendarItem { inText.text: parent.sevenString[1]}
        SepratorVertical {}

        CalendarItem { inText.text: parent.sevenString[2]}
        SepratorVertical {}

        CalendarItem { inText.text: parent.sevenString[3]}
        SepratorVertical {}

        CalendarItem { inText.text: parent.sevenString[4]}
        SepratorVertical {}

        CalendarItem { inText.text: parent.sevenString[5]}
        SepratorVertical {}

        CalendarItem { inText.text: parent.sevenString[6]}
    }
}
