import QtQuick 2.1
import "calendar_core.js" as CalendarCore
import Deepin.Widgets 1.0

Item {
    id: calendarWidget
    width: 308
    height: 240
    property var clickedDateObject: globalDate
    property var cur_calendar;
    property var pre_calendar;
    property var next_calendar;
    property bool slideStop: true

    function monthChange(dateValue){
        var d = new Date(dateValue)
        if (d > cur_calendar.clickedDateObject && slideStop){
            //var next_d = CalendarCore.getDateWidthMonthStep(cur_calendar.clickedDateObject, 1)
            next_calendar = calendarSlideBox.createCanlendar(d, "next")
            next_calendar.visible = true
            if (!toNextMonth.running && !toPreviousMonth.running){
                toNextMonth.restart()
            }
            clickedDateObject = d
        }
        else if (d < cur_calendar.clickedDateObject && slideStop){
            //var pre_d = CalendarCore.getDateWidthMonthStep(cur_calendar.clickedDateObject, -1)
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

            //for (var i=0; i<cur_calendar.debug_dates.length; i++){
                //console.log(cur_calendar.debug_dates[i].dateValue)
            //}
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

            //for (var i=0; i<cur_calendar.debug_dates.length; i++){
                //console.log(cur_calendar.debug_dates[i].dateValue)
            //}
        }
    }

    DBaseLine {
        id: dateBoxAdjustment

        height: 38
        color: "#1a1b1b"
        anchors.top: parent.top

        leftLoader.sourceComponent: DTextButton {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            text: dsTr("Today")

            onClicked: {
                monthChange(CalendarCore.dateToString(new Date()))
            }
        }

        rightLoader.sourceComponent: Component {
            // start row
            Row {
                height: dateBoxAdjustment.height
                width: (decreaseYearButton.width + decreaseMonthButton.width + 
                        monthAdjustmentBox.width + yearAdjustmentBox.width + 
                        increaseYearButton.width + increaseMonthButton.width)

                OpacityImageButton {
                    id: decreaseYearButton

                    source: 'images/arrow_left_white.png'
                    anchors.verticalCenter: parent.verticalCenter

                    onClicked: { 
                        var month_str = monthAdjustment.monthNumber;
                        if(monthAdjustment.monthNumber < 10){
                            month_str = "0" + month_str
                        }
                        var new_date_str = (yearAdjustment.yearNumber-1) + "-" + 
                            month_str + "-" + "01";
                        monthChange(new_date_str)
                    }
                }

                Item {
                    id: yearAdjustmentBox
                    anchors.verticalCenter: parent.verticalCenter
                    width: yearAdjustment.width + 6
                    height: parent.height

                    Text {
                        id: yearAdjustment
                        anchors.centerIn: parent
                        color: dconstants.fgColor
                        font.pixelSize: 12

                        property int yearNumber: Number(clickedDateObject.getFullYear())

                        text: String(yearNumber)
                    }
                        
                }

                OpacityImageButton {
                    id: increaseYearButton
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin: 6

                    source: "images/arrow_right_white.png"

                    onClicked: { 
                        var month_str = monthAdjustment.monthNumber;
                        if(monthAdjustment.monthNumber < 10){
                            month_str = "0" + month_str
                        }
                        var new_date_str = (yearAdjustment.yearNumber+1) + "-" + 
                            month_str + "-" + "01";
                        monthChange(new_date_str)
                    }
                }

                OpacityImageButton {
                    id: decreaseMonthButton
                    anchors.verticalCenter: parent.verticalCenter

                    source: "images/arrow_left_white.png"

                    onClicked: { 
                        if (monthAdjustment.monthNumber == 1) {
                            var new_monthNumber = 12
                            var new_yearNumber = yearAdjustment.yearNumber - 1
                        }
                        else {
                            var new_monthNumber = monthAdjustment.monthNumber - 1 
                            var new_yearNumber = yearAdjustment.yearNumber
                        }
                        if(new_monthNumber<10){
                            new_monthNumber = "0" + new_monthNumber
                        }
                        var new_date_str = new_yearNumber + "-" + 
                            new_monthNumber + "-" + "01";
                        //updateDates(new_date_str);
                        monthChange(new_date_str)
                    }
                }

                Item {
                    id: monthAdjustmentBox
                    anchors.verticalCenter: parent.verticalCenter
                    width: monthAdjustment.width + 6
                    height: parent.height

                    Text {
                        id: monthAdjustment
                        anchors.centerIn: parent
                        color: dconstants.fgColor
                        font.pixelSize: 12

                        property int monthNumber: Number(clickedDateObject.getMonth() + 1)
                        text: CalendarCore.CONST.months[monthNumber - 1]
                    }
                }

                OpacityImageButton {
                    id: increaseMonthButton
                    anchors.verticalCenter: parent.verticalCenter

                    source: "images/arrow_right_white.png"

                    onClicked: {
                        if (monthAdjustment.monthNumber == 12) {
                            var new_monthNumber = 1
                            var new_yearNumber = yearAdjustment.yearNumber + 1
                        }
                        else {
                            var new_monthNumber = monthAdjustment.monthNumber + 1
                            var new_yearNumber = yearAdjustment.yearNumber
                        }
                        if(new_monthNumber<10){
                            new_monthNumber = "0" + new_monthNumber
                        }
                        var new_date_str = new_yearNumber + "-" + 
                            new_monthNumber + "-" + "01"
                        //updateDates(new_date_str)
                        monthChange(new_date_str)
                    }
                }
            }
            // end row
        }
    }

    /***
    Rectangle {
        anchors.left: parent.left
        width: parent.width

        Rectangle {
            //left border
            anchors.top: parent.top
            anchors.left: parent.left
            width: 1
            height: parent.height
            color: "#303132"
        }

        Rectangle {
            //right border
            anchors.top: parent.top
            anchors.right: parent.right
            width: 1
            height: parent.height
            color: "#120f10"
        }

    }
    ***/

    DSeparatorHorizontal {
        anchors.top: dateBoxAdjustment.bottom
        width: parent.width
    }

    Rectangle {
        id: calendarSlideBox
        width: parent.width
        anchors.top: dateBoxAdjustment.bottom
        anchors.topMargin: 1
        anchors.left: parent.left
        property var component

        Component.onCompleted: {
            component = Qt.createComponent("CalendarComponent.qml");
            var cur_d = clickedDateObject
            cur_calendar = createCanlendar(cur_d, '');
            var pre_d = CalendarCore.getDateWidthMonthStep(cur_calendar.clickedDateObject, -1)
            var next_d = CalendarCore.getDateWidthMonthStep(cur_calendar.clickedDateObject, 1)
            pre_calendar = createCanlendar(pre_d, "previous")
            next_calendar = createCanlendar(next_d, "next")
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
