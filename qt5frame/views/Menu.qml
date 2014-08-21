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
import QtQuick.Window 2.1
import Deepin.Widgets 1.0

DWindowFrame{
    id: popupWindow
    clip: true
    z: 90

    property int innerWidth: 50
    property real posX: 0
    property real posY: 0
    property int shadowOffset: 12

    property alias currentIndex: completeView.currentIndex

    x: posX - shadowOffset
    y: posY - shadowOffset

    width: innerWidth + shadowOffset * 2
    height: completeViewBox.height + 32
    visible: false

    property var labels: []
    property Item requestMenuItem: popupWindow

    function menuSelect(i){
    }

    Item {
        id: completeViewBox
        anchors.centerIn: parent
        width: parent.width - 6
        height: completeView.height
        clip: true

        ListView {
            id: completeView
            width: parent.width
            height: childrenRect.height
            model: labels
            delegate: MenuItem {
                text: labels[index]
                onSelectAction:{
                    popupWindow.visible = false
                    popupWindow.requestMenuItem.menuSelect(index)
                }
            }
            clip: true
            interactive: true
        }
    }
}
