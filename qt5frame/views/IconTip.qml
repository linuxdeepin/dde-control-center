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
import Deepin.Locale 1.0
import Deepin.Widgets 1.0

Window {
    id: trayIconTip
    width: trayIconTipArrowRect.width
    height: trayIconTipArrowRect.height
    flags: Qt.Popup | Qt.WindowStaysOnTopHint

    property alias text: trayIconTipText.text
    property alias timeOutHide: hide
    property bool isHover: false

    color: Qt.rgba(0, 0, 0, 0)

    onVisibleChanged: {
        if(visible){
            freshBackground.restart()
        }
    }

    Timer{
        id: freshBackground
        interval: 100
        repeat: true
        property int repeatCount: 0

        onTriggered: {
            if(repeatCount > 10){
                freshBackground.stop()
                freshBackground.repeat = false
            }
            else{
                repeatCount += 1
                if (trayIconTip.color == Qt.rgba(0, 0, 0, 0)){
                    trayIconTip.color = Qt.rgba(0, 0, 0, 0.01)
                }
                else{
                    trayIconTip.color = Qt.rgba(0, 0, 0, 0)
                }
            }
        }
    }

    Timer {
        id: hide
        interval: 200
        running: false
        repeat: false
        onTriggered: {
            if (!trayIconTip.isHover){
                trayIconTip.visible = false
            }
        }
    }

    Behavior on y {
        SmoothedAnimation { duration: 300 }
        enabled: trayIconTip.visible
    }

    Row {
        id: trayIconTipArrowRect

        Image{
            source: "images/tooltip_left.png"
        }

        Image{
            width: trayIconTipText.width + 20
            source: "images/tooltip_middle.png"
        }
        
        Image{
            source: "images/tooltip_right.png"
        }

    }

    Text {
        id: trayIconTipText
        anchors.verticalCenter: trayIconTipArrowRect.verticalCenter
        anchors.verticalCenterOffset: 1
        anchors.horizontalCenter: trayIconTipArrowRect.horizontalCenter
        anchors.horizontalCenterOffset: - 4
        font.pixelSize: 13
        color: "white"
    }
}
