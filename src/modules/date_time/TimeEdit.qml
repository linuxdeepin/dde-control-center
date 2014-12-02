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

Row {
    width: childrenRect.width
    height: parent.height
    anchors.horizontalCenter: parent.horizontalCenter
    spacing: 10

    function init() {
        hoursEdit.text = globalDate.getHours() < 10 ? "0" + globalDate.getHours() : globalDate.getHours()
        minutesEdit.text = globalDate.getMinutes() < 10 ? "0" + globalDate.getMinutes() : globalDate.getMinutes()
        hoursEdit.forceActiveFocus()
    }

    signal accepted

    property string currentTime: hoursEdit.text + ":" + minutesEdit.text + ":0"
    property string currentHour: hoursEdit.text.trim()
    property string currentMin: minutesEdit.text.trim()

    Item {
        width: hoursEdit.width
        height: parent.height

        DArrowButton {
            visible: hoursEdit.activeFocus
            direction: "up"
            anchors.bottom: hoursEdit.top
            anchors.bottomMargin: 6
            anchors.horizontalCenter: hoursEdit.horizontalCenter

            onClicked: {
                hoursEdit.increase()
            }
        }

        TimeInput{
            id: hoursEdit
            anchors.verticalCenter: parent.verticalCenter
            min: 0
            max: 23
            activeFocusOnTab: true
            onToNext: {
                minutesEdit.forceActiveFocus()
            }

            onAccepted: parent.accepted()
        }

        DArrowButton {
            visible: hoursEdit.activeFocus
            direction: "down"
            anchors.top: hoursEdit.bottom
            anchors.topMargin: 6
            anchors.horizontalCenter: hoursEdit.horizontalCenter
            onClicked: {
                hoursEdit.decrease()
            }
        }
    }

    DLabel {
        id: secondColon
        font.family: fixedFont.name
        height: parent.height
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 55
        color: "white"
        text: ":"
    }

    Item {
        width: minutesEdit.width
        height: parent.height

        DArrowButton {
            visible: minutesEdit.activeFocus
            direction: "up"
            anchors.bottom: minutesEdit.top
            anchors.bottomMargin: 6
            anchors.horizontalCenter: minutesEdit.horizontalCenter
            onClicked: {
                minutesEdit.increase()
            }
        }

        TimeInput{
            id: minutesEdit
            anchors.verticalCenter: parent.verticalCenter
            min: 0
            max: 59
            activeFocusOnTab: true
            onToNext: {
                hoursEdit.forceActiveFocus()
            }
            onAccepted: parent.accepted()
        }

        DArrowButton {
            visible: minutesEdit.activeFocus
            direction: "down"
            anchors.top: minutesEdit.bottom
            anchors.topMargin: 6
            anchors.horizontalCenter: minutesEdit.horizontalCenter
            onClicked: {
                minutesEdit.decrease()
            }
        }
    }
}
