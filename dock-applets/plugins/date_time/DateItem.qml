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
import Deepin.Widgets 1.0

Item {
    id: dateItem
    width: itemWidth
    height: itemHeight

    property color borderColor: Qt.rgba(1, 1, 1, 0.2)

    property var curDate: new Date(dateValue)
    property var dateValueArray: dateValue.split("-")

    property var lunarDayInfo: dbusLunarCalendar.GetLunarInfoBySolar(parseInt(dateValueArray[0]), parseInt(dateValueArray[1]), parseInt(dateValueArray[2]))
    property var hasFestival: lunarDayInfo[1] && (lunarDayInfo[0][8] || lunarDayInfo[0][9])

    Item {
        id: content
        width: itemWidth - 1
        height: itemHeight - 1

        Rectangle {
            anchors.centerIn: parent
            width: diameter
            height: diameter
            radius: diameter/2
            color: "#0064fa"
            visible: dateItem.GridView.view.currentDateStr == dateValue

            property int diameter: parent.height
        }

        DssH2 {
            id: inText
            anchors.top: parent.top
            anchors.topMargin: lunarDayLabel.visible ? 2 : (parent.height - height)/2
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 14
            text: dayText
            color: isGrey ? "#4f4f4f": dconstants.fgColor
        }

        DssH3 {
            id: lunarDayLabel
            anchors.top: inText.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            font.pixelSize: 9
            color: isGrey ? "#4f4f4f": dconstants.fgColor
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
                if(dssLocale.lang == "zh_CN"){
                    lunarDayLabel.visible = true
                }
            }
        }

    }

    //Rectangle {
        //id: rightBorder
        //anchors.left: content.right
        //width: 1
        //height: parent.height
        //color: borderColor
        //visible: (index + 1) % 7 != 0
    //}

    //Rectangle {
        //id: bottomBorder
        //anchors.top: content.bottom
        //width: parent.width
        //height: 1
        //color: borderColor
    //}
}
