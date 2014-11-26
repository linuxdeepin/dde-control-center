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

Window {
    id: identifyWindow
    color: "transparent"
    flags: Qt.Tool | Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint
    width: monitorName.width + 20
    height: monitorName.height + 20

    property var monitorObject

    function showWindow(){
        identifyWindow.show()
        repaintTimer.restart()
    }

    x: monitorObject.x
    y: monitorObject.y

    Rectangle {
        anchors.fill: parent
        color: "yellow"
        border.width: 2
        border.color: "black"
    }

    DLabel {
        id: monitorName
        anchors.centerIn: parent
        text: monitorObject.name
        font.pixelSize: monitorObject.width/50
        font.weight: Font.DemiBold
        color: "black"
        opacity: 0.8
    }

    Timer {
        id: repaintTimer
        running: false
        interval: 300
        repeat: true
        onTriggered: {
            if(monitorName.opacity < 1){
                monitorName.opacity += 0.1
            }
            else{
                repaintTimer.stop()
            }
        }
    }
}
