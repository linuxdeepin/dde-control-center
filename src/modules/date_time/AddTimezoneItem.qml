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
    id: addZoneItem
    width: parent.width
    height: 50

    property string pTimezone:timezone

    property int pOffset: timeOffset
    property string pOffsetName: getOffsetName(pOffset)

    property bool selected: false

    signal selectAction(string timezone, bool selected)

    DssH3 {
        id: cityText
        anchors.top: parent.top
        anchors.topMargin: 6
        anchors.left: parent.left
        anchors.leftMargin: 14
        color: "#e6e6e6"
        font.pixelSize: 12
        elide: Text.ElideRight
        height: 30
        width: parent.width - checkButton.width - 36

        Component.onCompleted: {
            cityText.text = reduceTextLengthWithFlag(getCityListByOffset(pOffset),font.pixelSize,width,",")
        }
    }

    DssH3 {
        id:offsetText
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 14
        text: pOffsetName
        color: "#b4b4b4"
        font.pixelSize: 11
        elide: Text.ElideRight
        height: 20
        width: parent.width - checkButton.width - 36

    }

    DssMultiAddCheckButton {
        id: checkButton
        anchors.right: parent.right
        anchors.rightMargin: 14
        anchors.verticalCenter: parent.verticalCenter
        onClicked: {
            parent.selected = !parent.selected
            selectAction(pTimezone, selected)
        }
    }

    MouseArea {
        anchors.fill: parent
        anchors.rightMargin: checkButton.width + 14
        onClicked: {
            parent.selected = !parent.selected
            checkButton.active = selected
            selectAction(pTimezone, selected)
        }
    }

    DSeparatorHorizontal {anchors.bottom: parent.bottom}
}
