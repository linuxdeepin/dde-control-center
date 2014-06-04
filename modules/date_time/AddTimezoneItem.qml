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
    width: parent.width
    height: 28
    anchors.left: parent.left
    anchors.leftMargin: 15
    anchors.right: parent.right
    anchors.rightMargin: 15

    property string itemValue: value
    property string itemLabel: label

    property bool selected: false

    signal selectAction(string itemValue, bool selected)

    DssH3 {
        id: nameText
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        text: itemLabel
        color: dconstants.fgColor
        font.pixelSize: 12
        elide: Text.ElideRight
        width: parent.width - checkButton.width - 36
    }

    DssMultiAddCheckButton {
        id: checkButton
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        onClicked: {
            parent.selected = !parent.selected
            selectAction(itemValue, selected)
        }
    }
}
