import QtQuick 2.1
import "calendar_core.js" as CalendarCore
import Deepin.Widgets 1.0

Column {
    id: calendarWidget
    width: 308
    height: childrenRect.height

    property var clickedDateObject: globalDate
    property bool slideStop: true
    property bool isToday: CalendarCore.dateToString(clickedDateObject) == CalendarCore.dateToString(globalDate)
    property var cur_calendar;
    property var pre_calendar;
    property var next_calendar;

    function monthChange(dateValue){
        var d = new Date(dateValue)
        if (d > cur_calendar.clickedDateObject && slideStop){
            next_calendar = calendarSlideBox.createCanlendar(d, "next")
            next_calendar.visible = true
            if (!toNextMonth.running && !toPreviousMonth.running){
                toNextMonth.restart()
            }
            clickedDateObject = d
        }
        else if (d < cur_calendar.clickedDateObject && slideStop){
            pre_calendar = calendarSlideBox.createCanlendar(d, "previous")
            pre_calendar.visible = true
            if (!toNextMonth.running && !toPreviousMonth.running){
                toPreviousMonth.restart()
            }
            clickedDateObject = d
        }
    }

    ParallelAnimation {
        id: toNextMonth
        onStarted: {
            slideStop = false
        }
        PropertyAnimation {
            target: cur_calendar
            properties: "x"
            to: calendarSlideBox.x - calendarSlideBox.width
            easing.type: Easing.InOutQuad 
            duration: 300
        }
        PropertyAnimation {
            target: next_calendar
            properties: "x"
            to: calendarSlideBox.x
            easing.type: Easing.InOutQuad 
            duration: 300
        }
        onStopped: {
            cur_calendar.destroy()
            cur_calendar = next_calendar
            slideStop = true
        }
    }

    ParallelAnimation {
        id: toPreviousMonth
        onStarted: {
            slideStop = false
        }
        PropertyAnimation {
            target: pre_calendar
            properties: "x"
            to: calendarSlideBox.x
            easing.type: Easing.InOutQuad 
            duration: 300
        }
        PropertyAnimation {
            target: cur_calendar
            properties: "x"
            to: calendarSlideBox.x + calendarSlideBox.width
            easing.type: Easing.InOutQuad 
            duration: 300
        }
        onStopped: {
            cur_calendar.destroy()
            cur_calendar = pre_calendar
            slideStop = true
        }
    }

    DBaseLine {
        id: dateBoxAdjustment
        height: 38
        color: "#1a1b1b"

        leftLoader.sourceComponent: YearMonthAdjustor {
            height: dateBoxAdjustment.height
            currentDateObject: calendarWidget.clickedDateObject
            onMonthChanged: calendarWidget.monthChange(newDateString)
        }

        rightLoader.sourceComponent: TextAction {
            anchors.verticalCenter: parent.verticalCenter
            text: dsTr("Show Today")
            visible: opacity != 0
            opacity: isToday ? 0 : 1
            onClicked: {
                if(CalendarCore.isSameMonth(globalDate, calendarWidget.clickedDateObject)){
                    calendarWidget.clickedDateObject = globalDate
                    calendarWidget.cur_calendar.clickedDateObject = globalDate
                }
                else{
                    calendarWidget.monthChange(CalendarCore.dateToString(globalDate))
                }
            }
        }

    }

    DSeparatorHorizontal {}

    Rectangle {
        id: calendarSlideBox
        width: parent.width
        height: cur_calendar.height
        property var component: Qt.createComponent("CalendarComponent.qml")

        Component.onCompleted: {
            var cur_d = clickedDateObject
            cur_calendar = createCanlendar(cur_d, '');

            pre_calendar = cur_calendar
            next_calendar = cur_calendar

            //var pre_d = CalendarCore.getDateWidthMonthStep(cur_calendar.clickedDateObject, -1)
            //pre_calendar = createCanlendar(pre_d, "previous")

            //var next_d = CalendarCore.getDateWidthMonthStep(cur_calendar.clickedDateObject, 1)
            //next_calendar = createCanlendar(next_d, "next")
        }

        function createCanlendar(d_obj, position){
            var calendar = calendarSlideBox.component.createObject(calendarSlideBox, {
                "clickedDateObject": d_obj
            })

            if (position == 'previous'){
                calendar.x = calendarSlideBox.x - calendarSlideBox.width;
                calendar.visible = false
            }
            else if (position == 'next') {
                calendar.x = calendarSlideBox.x + calendarSlideBox.width;
                calendar.visible = false
            }
            else{
                calendar.x = calendarSlideBox.x
            }
            return calendar
        }
    }
}
