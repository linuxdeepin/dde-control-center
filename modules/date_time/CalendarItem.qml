import QtQuick 2.1
import "calendar_core.js" as CalendarCore

Rectangle {
    id: calendarItem
    width: 44
    height: 30
    color: "#1a1b1b"
    //color: "transparent"

    property alias inText: inText
    property bool grey: isGrey
    property bool isClicked: false
    property var curDate: new Date(dateValue)

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

    Rectangle {
        id: clickedDateBackground
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width -1
        height: parent.height -1
        color: Qt.rgba(0, 144, 255, 0.2)
        visible: index == calendarItem.GridView.view.currentIndex && !isCurrentDate ? true : false
    }

    Rectangle {
        id: currentDateBackground
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width -1
        height: parent.height -1
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.rgba(21/255, 147/255, 1, 0.5) }
            GradientStop { position: 1.0; color: Qt.rgba(0, 144/255, 1, 1.0) }
        }
        //visible: inText.text == 1 ? true : false
        visible: isCurrentDate
    }


    Text {
        id: inText
        anchors.centerIn: parent

        font.pixelSize: 12
        color: grey ? "#444" : "white"
        text: curDate.getDate()
    }

    MouseArea {
        anchors.fill: parent
        onWheel: {
            if (wheel.angleDelta.y < 0){
                var new_date = CalendarCore.getDateWidthMonthStep(curDate, 1)
            }
            else {
                var new_date = CalendarCore.getDateWidthMonthStep(curDate, -1)
            }
            new_date = CalendarCore.setDateOne(new_date)
            var new_date_str = CalendarCore.dateToString(new_date)
            calendarWidget.monthChange(new_date_str)
        }
        onClicked: {
            if (grey){
                calendarWidget.monthChange(dateValue);
            }
            else{
                calendarItem.GridView.view.currentIndex = index
            }
        }
    }
}
