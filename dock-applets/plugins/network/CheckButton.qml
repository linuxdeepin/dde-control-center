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
import QtGraphicalEffects 1.0

DImageCheckButton{
    property url onImage: ""
    property url offImage: ""
    property alias deviceIndex:indexText.text

    anchors.verticalCenter: parent.verticalCenter
    inactivatedNormalImage: offImage
    inactivatedHoverImage: inactivatedNormalImage
    inactivatedPressImage: inactivatedNormalImage

    activatedNormalImage: onImage
    activatedHoverImage: activatedNormalImage
    activatedPressImage: activatedNormalImage



    Text {
        id:indexText
        anchors {right: parent.right; bottom: parent.bottom}
        height: 9
        width: 9
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color: "#ffffff"
        font.bold: true
        font.pixelSize: 9
    }

    Glow {
        anchors.fill: indexText
        radius: 5
        samples: 16
        color: "#000"
        spread: 0.8
        source: indexText
    }
}
