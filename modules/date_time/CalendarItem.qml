import QtQuick 2.1
import QtGraphicalEffects 1.0
import "calendar_core.js" as CalendarCore

Rectangle {
    id: calendarItem
    color: "#1a1b1b"
    
    property string currentDateString: ""

    property bool isCurrentDate: currentDateString == dateValue
    property alias inText: inText
    property bool grey: isGrey
    property bool isClicked: false
    property var curDate: new Date(dateValue)
    property var lunarDay: windowView.getLunarDay(dateValue)

    // Start border
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
    // End of border


    Item{
        id: contentFrame
        anchors.centerIn: parent
        width: parent.width - 2
        height: parent.height - 2

        Rectangle {
            id: currentDateBackground
            anchors.fill: parent
            color: "#1a1b1b"
            visible: false
        }

        InnerShadow {
            source: currentDateBackground
            anchors.fill: currentDateBackground
            radius: 4
            samples: 10
            color: "#003A5F"
            horizontalOffset: 0
            verticalOffset: -3
            spread: 0.5
            visible: isCurrentDate
        }

        Rectangle {
            id: clickedDateBackground
            anchors.fill: parent
            color: "#003A5F"
            visible: index == calendarItem.GridView.view.currentIndex ? true : false
        }

        Item {
            anchors.fill: parent

            Text {
                id: inText
                anchors.top: parent.top
                anchors.topMargin: lunarDayLabel.visible ? 2 : (parent.height - height)/2
                anchors.horizontalCenter: parent.horizontalCenter

                font.pixelSize: 16
                color: grey ? "#444" : dconstants.fgColor
                text: curDate.getDate()
            }

            Text {
                id: lunarDayLabel
                anchors.top: inText.bottom
                //anchors.topMargin: 2
                anchors.horizontalCenter: parent.horizontalCenter

                font.pixelSize: 10
                color: {
                    if(lunarDay[2]){
                        return Qt.rgba(0, 144/255, 1, 1.0)
                    }
                    else if(grey){
                        return "#444"
                    }
                    return dconstants.fgColor
                }
                text: lunarDay[2] ? lunarDay[2] : lunarDay[0]
                visible: false

                Component.onCompleted: {
                    if(dsslocale.lang == "zh"){
                        lunarDayLabel.visible = true
                    }
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
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
                //calendarItem.GridView.view.currentIndex = index
                var myDate = new Date(dateValue)
                calendarWidget.clickedDateObject = myDate
                calendarWidget.cur_calendar.clickedDateObject = myDate
            }
        }
        onEntered: {
            var tipStr = ""
            if(dsslocale.lang == "zh"){
                tipStr = lunarDay[1]
            }
            else{
                tipStr = dateValue
            }
            toolTip.showTip(tipStr)
        }
        onExited: toolTip.hideTip()
    } // End of mousearea

}
