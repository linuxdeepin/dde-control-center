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

Rectangle {
    id: calendarComponent
    width: calendarItemCellWidth * 7
    height: calendarItemCellHeight * 6 + weekTitle.height
    color: "#1a1b1b"

    property alias weekTitleModel: weekTitleModel
    property alias datesGridView: datesGridView
    property alias datesModel: datesModel

    property int calendarItemCellWidth: 44
    property int calendarItemCellHeight: 38

    property var clickedDateObject: new Date()
    property var currentDateString: CalendarCore.dateToString(globalDate)

    onClickedDateObjectChanged: {
        updateSelectedIndex(CalendarCore.dateToString(clickedDateObject))
    }

    function updateSelectedIndex(date_str){
        for(var i=0; i < datesModel.count; i++){
            if(datesModel.get(i)["dateValue"] == date_str){
                datesGridView.currentIndex = i
                return
            }
        }
    }
    
    function initDates(date_str) {
        var dates = CalendarCore.getDates(date_str);
        datesModel.clear()
        for (var i=0; i < dates.length; i++){
            datesModel.append(dates[i])
            if (dates[i].dateValue == date_str) {
                datesGridView.currentIndex = i
            }
        }
        //datesGridView.height = (parseInt(dates.length/7) + 1) * datesGridView.cellHeight
    }

    ListView {
        id: weekTitle
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        height: 30
        delegate: CalendarTitleItem {}
        model: ListModel {id: weekTitleModel}
        orientation: ListView.Horizontal
        boundsBehavior: Flickable.StopAtBounds

        Component.onCompleted: {
            weekTitleModel.append({"dayText": dsTr("Sun"), "isHighlight": false})
            weekTitleModel.append({"dayText": dsTr("Mon"), "isHighlight": false})
            weekTitleModel.append({"dayText": dsTr("Tue"), "isHighlight": false})
            weekTitleModel.append({"dayText": dsTr("Wed"), "isHighlight": false})
            weekTitleModel.append({"dayText": dsTr("Thu"), "isHighlight": false})
            weekTitleModel.append({"dayText": dsTr("Fri"), "isHighlight": false})
            weekTitleModel.append({"dayText": dsTr("Sat"), "isHighlight": false})
        }
    }

    GridView {
        id: datesGridView
        anchors.top: weekTitle.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: cellWidth * 7
        height: childrenRect.height
        cellWidth: calendarItemCellWidth
        cellHeight: calendarItemCellHeight
        model: ListModel {id: datesModel}
        delegate: CalendarItem {
            width: calendarItemCellWidth
            height: calendarItemCellHeight
            currentDateString: calendarComponent.currentDateString
        }
        focus: true
        currentIndex: -1
        boundsBehavior: Flickable.StopAtBounds

        Component.onCompleted: { 
            initDates(CalendarCore.dateToString(clickedDateObject))
        }

        function getDelegateInstanceAt(index) {
            var item = contentItem.children[index];
            return item;
        }
    }
}
