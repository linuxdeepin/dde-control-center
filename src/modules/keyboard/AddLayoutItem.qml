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

Rectangle {
    id: addLayoutItem
    width: parent.width
    height: 28
    color: dconstants.contentBgColor

    property string itemId: item_id
    property string layoutName: label

    property bool selected: false

    DssH3 {
        id: nameText
        anchors.left: parent.left
        anchors.leftMargin: 18
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 12
        text: layoutName
        width: parent.width - addButton.width - 40
        elide: Text.ElideRight
    }

    DssMultiAddCheckButton {
        id: addButton
        anchors.right: parent.right
        anchors.rightMargin: 18
        anchors.verticalCenter: parent.verticalCenter

        onClicked: {
            parent.selected = !parent.selected
        }
    }

    MouseArea {
        anchors.fill: parent
        anchors.rightMargin: addButton.width + 18
        onClicked :{
            parent.selected = !parent.selected
            addButton.active = selected
        }
    }
}

