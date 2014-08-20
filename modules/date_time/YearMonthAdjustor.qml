/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Kaisheng Ye
**
**  Author:     Kaisheng Ye <kaisheng.ye@gmail.com>
**  Maintainer: Kaisheng Ye <kaisheng.ye@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

import QtQuick 2.1
import "calendar_core.js" as CalendarCore
import Deepin.Widgets 1.0

Row {
    height: parent.height
    width: childrenRect.width

    property string dateValue: ""
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
            property int yearNumber: Number(dateValue.split("-")[0])
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

            property int monthNumber: Number(dateValue.split("-")[1])
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
