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
import QtGraphicalEffects 1.0
import Deepin.Widgets 1.0

Item {
    id: previewImage

    property int radius: 6
    property url imageSource: ""

    signal clicked(int index, string side)

    Image{
        id: bg
        source: imageSource
        width: parent.width
        height: parent.height
        smooth: true
        visible: false
    }

    Rectangle{
        id: borderRect
        width: parent.width
        height: parent.height
        radius: previewImage.radius
        smooth: true
        visible: false
    }

    OpacityMask{
        anchors.fill: bg
        source: bg
        maskSource: borderRect
    }

    MouseArea{
        width: parent.width/2
        height: parent.height
        hoverEnabled: true
        property url leftCursor: "images/left_normal.png"
        onClicked: {
            parent.clicked(index, "left")
        }

        onEntered: mainObject.setCustomCursor(leftCursor)
        onExited: mainObject.clearCustomCursor()
    }

    MouseArea{
        width: parent.width/2
        height: parent.height
        anchors.right: parent.right
        hoverEnabled: true
        property url rightCursor: "images/right_normal.png"
        onClicked: {
            parent.clicked(index, "right")
        }
        onEntered: mainObject.setCustomCursor(rightCursor)
        onExited: mainObject.clearCustomCursor()
    }
}
