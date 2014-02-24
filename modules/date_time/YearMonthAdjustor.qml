import QtQuick 2.1
import "calendar_core.js" as CalendarCore
import Deepin.Widgets 1.0

Row {
    height: parent.height
    width: childrenRect.width

    property var currentDateObject: new Date()
    signal monthChanged(var newDateString )

    DOpacityImageButton {
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
            monthChanged(new_date_str)
        }
    }

    Item {
        id: yearAdjustmentBox
        anchors.verticalCenter: parent.verticalCenter
        width: yearAdjustment.implicitWidth + 6
        height: parent.height

        Text {
            id: yearAdjustment
            anchors.centerIn: parent
            color: dconstants.fgColor
            font.pixelSize: 12
            property int yearNumber: Number(currentDateObject.getFullYear())
            text: String(yearNumber)
        }
    }

    DOpacityImageButton {
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
            monthChanged(new_date_str)
        }
    }

    DOpacityImageButton {
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
            monthChanged(new_date_str)
        }
    }

    Item {
        id: monthAdjustmentBox
        anchors.verticalCenter: parent.verticalCenter
        width: monthAdjustment.implicitWidth + 6
        height: parent.height

        Text {
            id: monthAdjustment
            anchors.centerIn: parent
            color: dconstants.fgColor
            font.pixelSize: 12

            property int monthNumber: Number(currentDateObject.getMonth() + 1)
            text: monthNumber < 10 ? "0" + monthNumber : monthNumber
        }
    }

    DOpacityImageButton {
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
            monthChanged(new_date_str)
        }
    }
}
