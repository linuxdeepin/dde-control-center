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
import Deepin.Widgets 1.0

Item {
    id: wrapper
    width: wrapper.ListView.view.width; height: 26

    property alias text: label.text

    Rectangle {
        color: itemOnHover ? "#141414" : "#191919"
        anchors.fill: parent
    }

    Image {
        id: headImg
        source: itemOnHover ? "images/select-dark-hover.png" : "images/select-dark.png"
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 5
        visible: index == 0
    }

    DLabel {
        id: label
        anchors.left: parent.left
        anchors.leftMargin: 28
        anchors.verticalCenter: parent.verticalCenter
        text: "text " + index
        font.pixelSize: 12
        font.family: label.text
        color: itemOnHover ? dconstants.activeColor : dconstants.fgColor
    }
}
