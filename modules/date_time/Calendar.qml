import QtQuick 2.1
import "calendar_core.js" as CalendarCore

Item {
    id: calendarWidget
    width: 308
    property var clickedDateObject: new Date()
    property string clickedDate: CalendarCore.dateToString(clickedDateObject)
    property var cur_calendar;
    property var pre_calendar;
    property var next_calendar;

    signal monthChange(string dateValue);

    function updateDates(date_str) {
        clickedDateObject = new Date(date_str);

        var currentDateString = CalendarCore.dateToString(new Date())
        var dates = CalendarCore.getDates(date_str);

        cur_calendar.datesModule.clear()
        for (var i=0; i < dates.length; i++){
            cur_calendar.datesModule.append(dates[i])
            if (dates[i].dateValue == date_str) {
                cur_calendar.datesGridView.currentIndex = i
            }
        }
        cur_calendar.datesGridView.height = (parseInt(dates.length/7) + 1) * cur_calendar.datesGridView.cellHeight
    }

    Rectangle {
        id: dateBoxAdjustment
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width
        height: 28
        color: "#1a1b1b"

        Rectangle {
            //left border
            anchors.top: parent.top
            anchors.left: parent.left
            width: 1
            height: parent.height
            color: "#303132"
        }

        Rectangle {
            //left border
            anchors.top: parent.top
            anchors.right: parent.right
            width: 1
            height: parent.height
            color: "#120f10"
        }

        ImageButton {
            id: decreaseYearButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 16

            nomralImage: 'images/arrow_left_normal.png'
            hoverImage: 'images/arrow_left_hover.png'
            pressImage: 'images/arrow_left_press.png'

            onClicked: { 
                var new_date_str = (yearAdjustment.yearNumber-1) + "-" + 
                    monthAdjustment.monthNumber + "-" + 1;
                updateDates(new_date_str);
            }
        }

        Text {
            id: yearAdjustment
            property int yearNumber: Number(clickedDateObject.getFullYear())
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: decreaseYearButton.right
            anchors.leftMargin: 12

            color: textColor
            font.pixelSize: 12
            text: yearNumber + "年"
        }

        ImageButton {
            id: increaseYearButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: yearAdjustment.right
            anchors.leftMargin: 12

            nomralImage: 'images/arrow_right_normal.png'
            hoverImage: 'images/arrow_right_hover.png'
            pressImage: 'images/arrow_right_press.png'

            onClicked: { 
                var new_date_str = (yearAdjustment.yearNumber+1) + "-" + 
                    monthAdjustment.monthNumber + "-" + 1;
                updateDates(new_date_str);
            }
        }

        ImageButton {
            id: decreaseMonthButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: monthAdjustment.left
            anchors.rightMargin: 12

            nomralImage: 'images/arrow_left_normal.png'
            hoverImage: 'images/arrow_left_hover.png'
            pressImage: 'images/arrow_left_press.png'

            onClicked: { 
                if (monthAdjustment.monthNumber == 1) {

                    var new_monthNumber = 12
                }
                else {
                    var new_monthNumber = monthAdjustment.monthNumber - 1 
                }
                var new_date_str = yearAdjustment.yearNumber + "-" + 
                    new_monthNumber + "-" + 1;
                updateDates(new_date_str);
            }
        }

        Text {
            id: monthAdjustment
            property int monthNumber: Number(clickedDateObject.getMonth() + 1)
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: increaseMonthButton.left
            anchors.rightMargin: 12

            color: textColor
            font.pixelSize: 12
            text: monthNumber + "月"
        }

        ImageButton {
            id: increaseMonthButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 16

            nomralImage: 'images/arrow_right_normal.png'
            hoverImage: 'images/arrow_right_hover.png'
            pressImage: 'images/arrow_right_press.png'

            onClicked: {
                if (monthAdjustment.monthNumber == 12) {

                    var new_monthNumber = 1
                }
                else {
                    var new_monthNumber = monthAdjustment.monthNumber + 1
                }
                var new_date_str = yearAdjustment.yearNumber + "-" + 
                    new_monthNumber + "-" + 1
                updateDates(new_date_str)
            }
        }
    }

    SepratorHorizontal {
        anchors.top: dateBoxAdjustment.bottom
        width: parent.width
    }

    Rectangle {
        id: calendarSlideBox
        width: parent.width
        anchors.top: dateBoxAdjustment.bottom
        anchors.topMargin: 1
        anchors.left: parent.left

        Component.onCompleted: {
            cur_calendar = createCanlendar(new Date(), '');
        }

        function createCanlendar(d_obj, position){
            var component = Qt.createComponent("CalendarComponent.qml");
            var calendar = component.createObject(calendarSlideBox, {
                "clickedDateObject": d_obj
            })

            if (position == 'previous'){
                calendar.x = calendarSlideBox.x - calendarSlideBox.width;
            }
            else if (position == 'next') {
                calendar.x = calendarSlideBox.x + calendarSlideBox.width;
            }
            else{
                calendar.x = calendarSlideBox.x
            }
            return calendar
        }
    }
}
