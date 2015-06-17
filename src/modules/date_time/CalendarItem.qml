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
import QtGraphicalEffects 1.0
import "calendar_core.js" as CalendarCore
import Deepin.Widgets 1.0

Rectangle {
    id: calendarItem
    color: "#1a1b1b"

    property alias inText: inText
    property bool grey: isGrey
    property var dateValueArray: dateValue.split("-")
    property string componentMonth: ""

    property var lunarDayInfo: dbusLunarCalendar.GetLunarInfoBySolar(parseInt(dateValueArray[0]), parseInt(dateValueArray[1]), parseInt(dateValueArray[2]))
    property var hasFestival: lunarDayInfo[1] && (lunarDayInfo[0][8] || lunarDayInfo[0][9])

    signal itemClicked(string tooltip)
    signal itemEnter()
    signal itemExist()
    Item{
        id: contentFrame
        anchors.centerIn: parent
        width: parent.width - 2
        height: parent.height - 2

        Rectangle {
            id: clickedDateBackground
            width: diameter
            height: diameter
            anchors.centerIn: parent
            color: "#0064fa"
            visible: index == calendarItem.GridView.view.currentIndex && slideStop ? true : false
            radius: diameter/2

            property int diameter: parent.height
        }

        Item {
            anchors.fill: parent

            Text {
                id: inText
                anchors.top: parent.top
                anchors.topMargin: lunarDayLabel.visible ? 2 : (parent.height - height)/2
                anchors.horizontalCenter: parent.horizontalCenter

                font.pixelSize: 15
                color: grey ? "#444" : "white"
                text: dateValueArray[2]
            }

            DssH3 {
                id: lunarDayLabel
                anchors.top: inText.bottom
                //anchors.topMargin: 2
                anchors.horizontalCenter: parent.horizontalCenter

                font.pixelSize: 10
                color: {
                    if(hasFestival){
                        if (clickedDateBackground.visible){
                            return "#ffffff"
                        }
                        else{
                            return "#00BDFF"
                        }
                    }
                    else if(grey){
                        return "#444"
                    }
                    return DConstants.fgColor
                }
                text: {
                    var info = lunarDayInfo[0]
                    if(info[7]){
                        return info[7]
                    }
                    else if(info[4] == "初一"){
                        return info[3]
                    }
                    else{
                        return info[4]
                    }
                }
                visible: false

                Component.onCompleted: {
                    if(dsslocale.lang == "zh_CN"){
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
                var new_date_str = CalendarCore.getNextMonthDateValue(componentMonth + "-01")
            }
            else {
                var new_date_str = CalendarCore.getPreviousMonthDateValue(componentMonth + "-01")
            }
            calendarWidget.currentSelectedDateValue = new_date_str
            calendarWidget.initTooltip(new_date_str)
        }
        onClicked: {
            calendarWidget.currentSelectedDateValue = dateValue
            var tipStr = ""
            if(dsslocale.lang == "zh_CN"){
                if(lunarDayInfo[1]){
                    var info = lunarDayInfo[0]
                    tipStr = info[3] + info[4]
                    if(info[8]){
                        tipStr += "\n" + info[8]
                    }
                    if(info[9]){
                        tipStr += "\n" + info[9]
                    }
                }

            }
            calendarItem.itemClicked(tipStr)
        }
        onEntered: {
            calendarItem.itemEnter()
        }
    } // End of mousearea

}
