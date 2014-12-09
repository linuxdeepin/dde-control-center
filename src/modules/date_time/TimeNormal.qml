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
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

Item {
    id: digitalTime
    width: 150
    height: 38

    property bool use24Hour: false
    property bool mouseAreaVisible: true

    property int fontSize: 55
    property color fontColor: "white"

    Row {
        spacing: 4
        anchors.verticalCenter: parent.verticalCenter
        width: childrenRect.width
        height: childrenRect.height
        x: (parent.width - width)/2

        Behavior on x{
            PropertyAnimation { duration: 200 }
        }

        DLabel {
            id: hoursText
            font.family: fixedFont.name
            font.pixelSize: fontSize
            color: fontColor
            text: {
                var currentHour = globalDate.getHours()
                if(use24Hour){
                    return currentHour
                }
                else{
                    if(currentHour == 12 | currentHour == 0){
                        return 12
                    }
                    else{
                        return currentHour % 12
                    }
                }
            }
        }

        DLabel {
            id: secondColon
            font.family: fixedFont.name
            font.pixelSize: fontSize
            color: fontColor
            text: ":"
        }

        DLabel {
            id: minutesText
            font.family: fixedFont.name
            font.pixelSize: fontSize
            color: fontColor
            text: globalDate.getMinutes() < 10 ? "0" + globalDate.getMinutes() : globalDate.getMinutes()
        }
    }
}
