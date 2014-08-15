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

import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.DateAndTime 1.0
import DBus.Com.Deepin.Api.LunarCalendar 1.0
import "../widgets"
import "calendar_core.js" as CalendarCore

DockApplet{
    id: datetimeApplet
    title: "Date and Time"
    appid: "AppletDateTime"
    icon: "vcalendar" // analog

    property string currentClockType: "digital"
    onCurrentClockTypeChanged: {
        setData("clockType", currentClockType)
    }

    property int xEdgePadding: 18
    property int rootWidth: 260

    property var locale: Qt.locale()
    property var globalDate: new Date()
    onGlobalDateChanged: {
        weekTitleGridView.currentIndex = globalDate.getDay()
        datesGridView.currentDateStr = CalendarCore.dateToString(globalDate)
    }
    property var dbusDateAndTime: DateAndTime {
        onCurrentTimezoneChanged: {
            Date.timeZoneUpdated()
            globalDate = new Date()
        }
    }
    property var dbusLunarCalendar: LunarCalendar {}

    Timer {
        id: timeTicker
        running: true
        repeat: true
        interval: 1000
        onTriggered: {
            globalDate = new Date()
        }
    }

    Component.onCompleted: {
        setData("clockType", currentClockType)
    }

    onActivate: {
        showDateTime(0)
    }

    onNativeWindowDestroyed: {
        toggle_applet("date_time")
        toggle_applet("date_time")
    }

    onQt5ScreenDestroyed: {
        console.log("Recive onQt5ScreenDestroyed")
        mainObject.restartDockApplet()
    }

    property var typeNames: {
        "digital": dsTr("_Switch to analog"),
        "analog": dsTr("_Switch to digital")
    }

    function showDateTime(id){
        dbusControlCenter.ShowModule("date_time")
    }

    function hideDateTime(id){
        set_hide_applet("date_time")
    }

    function switchClockType(id){
        if(currentClockType == "digital"){
            currentClockType = "analog"
        }
        else{
            currentClockType = "digital"
        }
        var content_obj = unmarshalJSON(menu.content)
        content_obj.menuItems[id].itemText = typeNames[currentClockType]
        menu.content = marshalJSON(content_obj)
        print(id, currentClockType)
    }

    //menu: Menu{
        //Component.onCompleted: {
            //addItem(dsTr("_Run"), showDateTime);
            //addItem(typeNames[currentClockType], switchClockType);
        //}
    //}

    property int itemWidth: 37
    property int itemHeight: 36
    property int weekTitleHeight: 21

    window: DockQuickWindow {
        id: root
        width: rootWidth
        height: contentColumn.height
        color: "transparent"

        Column {
            id: contentColumn
            width: itemWidth * 7 - 1
            anchors.horizontalCenter: parent.horizontalCenter

            Item {
                id: topTitle
                width: parent.width - 16
                anchors.horizontalCenter: parent.horizontalCenter
                height: 38

                DssH2 {
                    text: globalDate.toLocaleDateString(locale)
                    anchors.verticalCenter: parent.verticalCenter
                }

                DssH2 {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    visible: false
                    property int fullYear: globalDate.getFullYear()
                    property int month: globalDate.getMonth() + 1
                    property int dateNumber: globalDate.getDate()
                    text: {
                        var info = dbusLunarCalendar.GetLunarInfoBySolar(fullYear, month, dateNumber)
                        return "农历" + info[0][3] + info[0][4]
                    }
                    Component.onCompleted: {
                        if(dssLocale.lang == "zh_CN"){
                            visible = true
                        }
                    }
                }
            }

            GridView {
                id: weekTitleGridView
                width: itemWidth * 7
                height: weekTitleHeight
                cellWidth: itemWidth
                cellHeight: weekTitleHeight
                boundsBehavior: Flickable.StopAtBounds
                model: ListModel {id: weekTitleModel}
                delegate: WeekTitleItem{}

                Component.onCompleted: {
                    weekTitleModel.append({"dayText": dsTr("Sun")})
                    weekTitleModel.append({"dayText": dsTr("Mon")})
                    weekTitleModel.append({"dayText": dsTr("Tue")})
                    weekTitleModel.append({"dayText": dsTr("Wed")})
                    weekTitleModel.append({"dayText": dsTr("Thu")})
                    weekTitleModel.append({"dayText": dsTr("Fri")})
                    weekTitleModel.append({"dayText": dsTr("Sat")})
                    currentIndex = globalDate.getDay()
                }
            }

            GridView {
                id: datesGridView
                width: itemWidth * 7
                height: itemHeight * 6
                cellWidth: itemWidth
                cellHeight: itemHeight
                model: ListModel { id: datesModel }
                delegate: DateItem {}
                boundsBehavior: Flickable.StopAtBounds
                focus: true
                property var currentDateStr: CalendarCore.dateToString(globalDate)

                property var lastCurrentDateStr: ""

                onCurrentDateStrChanged: {
                    if(lastCurrentDateStr != currentDateStr){
                        lastCurrentDateStr = currentDateStr
                        initDates(lastCurrentDateStr)
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
                }
            }

            Item {
                width: parent.width
                height: (rootWidth - parent.width)/2
            }

        }
        
    }
}
