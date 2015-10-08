/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 WanQing Yang
**
**  Author:     WanQing Yang <match.yangwanqing@gmail.com>
**  Maintainer: WanQing Yang <match.yangwanqing@gmail.com>
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
    id:imageLabel
    width: childrenRect.width
    height: 30

    property string normal_image:""
    property string hover_image:""
    property string press_image:""
    property alias label_text:identifyText.text

    signal clicked()

    Image {
        id:identifyImg
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        source: normal_image
    }

    Text {
        id:identifyText
        anchors.left: identifyImg.right
        anchors.leftMargin: 3
        anchors.verticalCenter: parent.verticalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        color: DPalette.fgColor
        font.pixelSize: 12
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            identifyImg.source = hover_image
            identifyText.color = DPalette.hoverColor
        }
        onExited: {
            identifyImg.source = normal_image
            identifyText.color = DPalette.fgColor
        }
        onPressed: {
            identifyImg.source = press_image
            identifyText.color = DPalette.activeColor

            imageLabel.clicked()
        }
        onReleased: {
            identifyImg.source = normal_image
            identifyText.color = DPalette.fgColor
        }

    }
}
