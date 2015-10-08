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

Rectangle {
    width: parent.width
    height: 28
    color: DPalette.contentBgColor

    property string title
    property string value
    property int fontSize: 12
    property int leftWidth: systemInfoModule.leftWidth
    property alias titleWidth: titleArea.contentWidth

    property bool valueWrapped: false

    Component.onCompleted: {
        if (dsslocale.lang == "en") {
            //fontSize = 11
            print(leftWidth)
        }
    }

     DLabel {
        id: titleArea
        width: leftWidth
        anchors.verticalCenter: parent.verticalCenter
        horizontalAlignment: Text.AlignRight
        font.pixelSize: fontSize
        text: title + " : "
    }

    DLabel {
        id: systemVersion
        anchors.left: titleArea.right
        anchors.leftMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: fontSize
        width: parent.width - leftWidth - 8
        text: value

        Component.onCompleted: {
            if (width < contentWidth){
                valueWrapped = true
                wrapMode = Text.WordWrap
                parent.height += 14
            }
        }
    }
}
