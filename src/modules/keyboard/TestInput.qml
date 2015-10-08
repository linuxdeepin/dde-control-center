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

DTextInput{
    property int cursorBlinkTime: 1200

    textInput.cursorDelegate: Rectangle {
        width: 1
        height: parent.height
        color: DPalette.fgColor
        opacity: cursorBlinkTimer.cursorOpacity
    }

    Timer{
        id: cursorBlinkTimer
        repeat: true
        running: true
        interval: parent.cursorBlinkTime/2
        property real cursorOpacity: 1

        onTriggered: {
            if(cursorOpacity == 1){
                cursorOpacity = 0
            }
            else{
                cursorOpacity = 1
            }
        }
    }
}
