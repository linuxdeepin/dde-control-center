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

Slider {
    id: slider
    width: 196
    property int grooveWidth: width - handleWidth
    property int grooveHeight: 8
    property int handleWidth: 12

    property int _buttonWidth: 0

    style: SliderStyle {
        groove: Rectangle {
            implicitWidth: grooveWidth + handleWidth
            implicitHeight: 2
            color: Qt.rgba(1, 1, 1, 0.3)

            Rectangle {
                width: slider.value/(slider.maximumValue - slider.minimumValue) * parent.width
                height: parent.height
                color: Qt.rgba(1, 1, 1, 0.5)
            }

        }
        handle: Rectangle {
            width: handleWidth
            height: handleWidth
            radius: handleWidth/2
        }
    }

}

