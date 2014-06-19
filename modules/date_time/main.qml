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

import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.DateAndTime 1.0
import DBus.Com.Deepin.Api.LunarCalendar 1.0
import DBus.Com.Deepin.Daemon.Search 1.0
import "calendar_core.js" as CalendarCore

DFlickable {
    id: dateTimeModule
    anchors.fill: parent

    contentWidth: width
    contentHeight: height < 768 ? 768 : height

    FontLoader { id: fixedFont; source: "MavenProLight-200.otf" }
    property var gDate: DateAndTime {
        onCurrentTimezoneChanged: {
            Date.timeZoneUpdated()
            globalDate = new Date()
        }
    }
    property var dconstants: DConstants {}
    property string lang: 'en_US'
    property var locale: Qt.locale()

    property var dbusLunarCalendar: LunarCalendar {}

    property var dbusSearch: Search {}

    property var globalDate: new Date()
    
    Component.onCompleted: {
        lang = dsslocale.lang
    }

    Timer {
        running: true
        repeat: true
        interval: 500
        onTriggered: { 
            dateTimeModule.globalDate= new Date()
        }
    }

    Column {
        id: beforeTimeZoneArea
        anchors.top: parent.top
        width: parent.width
        height: childrenRect.height

        DssTitle {
            text: modulesId.moduleLocaleNames["date_time"]
        }

        DSeparatorHorizontal {}

        TimeBox {
            id: timeBox
            use24Hour: twentyFourHourSetBox.active
            editable: !gDate.autoSetTime
        }

        DSeparatorHorizontal {}

        DSwitchButtonHeader {
            id: autoSetTimeBox
            text: dsTr("Automatic date and time")
            width: parent.width
            active: gDate.autoSetTime

            onClicked: {
                gDate.autoSetTime = active
                if(active && timeBox.inEdit){
                    timeBox.showTimeNormal()
                }
            }
        }

        DSeparatorHorizontal {}

        DSwitchButtonHeader {
            id: twentyFourHourSetBox
            text: dsTr("Use 24-hour clock")
            active: gDate.use24HourDisplay

            onClicked: {
                gDate.use24HourDisplay = active
            }
        }

        DSeparatorHorizontal {}
    }

    TimezoneArea {
        id: timeZoneArea
        anchors.top: beforeTimeZoneArea.bottom
        listAreaMaxHeight: parent.height - beforeTimeZoneArea.height - afterTimeZoneArea.height - 64
        onCurrentActionStateNameChanged: {
            if(currentActionStateName != ""){
                localeArea.expanded = false
            }
        }
    }

    Column {
        id: localeAreaColumn
        anchors.top: timeZoneArea.bottom
        width: parent.width

        DSeparatorHorizontal {}

        LocaleArea {
            id: localeArea
            listAreaMaxHeight: timeZoneArea.listAreaMaxHeight - 60
            onExpandedChanged: {
                if(expanded){
                    timeZoneArea.reset()
                }
            }
        }
    }

    Column {
        id: afterTimeZoneArea
        anchors.top: localeAreaColumn.bottom

        DSeparatorHorizontal {}

        DBaseLine {
            id: dateBoxTitle
            width: parent.width
            rightMargin: 10

            leftLoader.sourceComponent: DLabel {
                font.pixelSize: 13
                text: dsTr("Date")
            }

            rightLoader.sourceComponent: DTextButton {
                text: dsTr("Set Date")
                visible: opacity != 0
                opacity: calendarObj.isToday ? 0 : 1

                Behavior on opacity {
                    PropertyAnimation { duration: 200 }
                }

                onClicked: {
                    gDate.SetDate(CalendarCore.dateToString(calendarObj.clickedDateObject))
                }
            }
        }

        DSeparatorHorizontal {}

        Calendar { 
            id: calendarObj 
        }

        DSeparatorHorizontal {}
    }
}
