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

ScrollView {
    id: root
    frameVisible: false
    property int rightMargin
    Component.onCompleted: {
        viewport.anchors.rightMargin = 0
        __scroller.rightMargin = -3 + rightMargin
        flickableItem.interactive = true
    }

    style: ScrollViewStyle {
        handleOverlap: 0
        handle: Item {
            implicitWidth: 14
            implicitHeight: 26
            Rectangle {
                color: styleData.hovered ? Qt.rgba(66/255.0, 66/255.0, 70/255.0, 0.8) : Qt.rgba(100/255.0, 100/255.0, 100/255.0)
                anchors.fill: parent
                anchors.topMargin: 6
                anchors.leftMargin: 4
                anchors.rightMargin: 4
                anchors.bottomMargin: 6
                radius: 3
            }
        }
        scrollBarBackground: Item {
            implicitWidth: 14
            implicitHeight: 26
        }
        decrementControl: Item {}
        incrementControl: Item {}
    }
}
