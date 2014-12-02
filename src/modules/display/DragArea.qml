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

MouseArea {
    id: dragArea
    property variant window
    
    property bool isHover: false
    property bool isDoubleClick: false
    
    property real lastX: 0
    property real lastY: 0
    property real lastWindowX: 0
    property real lastWindowY: 0
    
    signal singleClicked
    
    onPressed: {
        isHover = false
        isDoubleClick = false
        
        var pos = window.getCursorPos()
        lastX = pos.x
        lastY = pos.y
        
        lastWindowX = window.x
        lastWindowY = window.y
    }
    
    onClicked: {
        if (!isHover) {
            clickTimer.restart()
        }
    }
    
    onDoubleClicked: {
        isDoubleClick = true
    }
    
    onPositionChanged: {
        isHover = true
        
        if (pressedButtons == Qt.LeftButton) {
            var pos = window.getCursorPos()
            window.x = lastWindowX + pos.x - lastX
            window.y = lastWindowY + pos.y - lastY
        }
    }
    
    Timer {
        id: clickTimer
        interval: 200
        repeat: false
        onTriggered: {
            if (!dragArea.isDoubleClick) {
                singleClicked()
            }
        }
    }
}
