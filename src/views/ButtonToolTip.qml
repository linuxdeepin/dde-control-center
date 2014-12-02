/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Wanqing Yang
**
**  Author:     Wanqing Yang <match.yangwanqing@gmail.com>
**  Maintainer: Wanqing Yang <match.yangwanqing@gmail.com>
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

Item {
    id:buttonToolTip
    height: tipText.implicitHeight
    width: tipText.implicitWidth
    state: "hideTip"
    clip: true

    property int animationDuration: 500
    property int autoHideInterval: -1
    property int delayShowInterval: -1
    property int targetX: 0

    states: [
        State {
            name: "showTip"
            PropertyChanges { target: buttonToolTip; width: tipText.implicitWidth; x: targetX }
        },
        State {
            name: "hideTip"
            PropertyChanges { target: buttonToolTip; width: 0;x: targetX + tipText.implicitWidth}

        }
    ]

    transitions:[
        Transition {
            from: "showTip"
            to: "hideTip"
            ParallelAnimation {
                NumberAnimation {target: buttonToolTip;property: "width";duration: animationDuration;easing.type: Easing.OutCubic}
                NumberAnimation {target: buttonToolTip;property: "x";duration: animationDuration;easing.type: Easing.OutCubic}
            }
        },
        Transition {
            from: "hideTip"
            to: "showTip"
            ParallelAnimation {
                NumberAnimation {target: buttonToolTip;property: "width"; duration: animationDuration; easing.type: Easing.OutCubic}
                NumberAnimation {target: buttonToolTip;property: "x";duration: animationDuration;easing.type: Easing.OutCubic}
            }
        }
    ]

    function resetState(){
        buttonToolTip.width = 0
        buttonToolTip.x = targetX + tipText.implicitWidth
    }

    function showToolTip(x,y,tips){
        resetState()
        buttonToolTip.visible = true

        tipText.text = tips
        buttonToolTip.targetX = x - tipText.implicitWidth
        buttonToolTip.x = x
        buttonToolTip.y = y

        delayShowTimer.stop()
        delayShowTimer.restart()
    }

    function hideToolTip(){
        delayShowTimer.stop()
        buttonToolTip.state = "hideTip"
    }

    Timer {
        id:autoHideTimer
        repeat: false
        running: false
        interval:autoHideInterval
        onTriggered: hideToolTip()
    }

    Timer {
        id:delayShowTimer
        repeat: false
        running: false
        interval: delayShowInterval > 0 ? delayShowInterval : 0
        onTriggered: {
            buttonToolTip.state = "showTip"

            if (autoHideInterval > 0){
                autoHideTimer.stop()
                autoHideTimer.start()
            }
        }
    }

    Text {
        id:tipText
        color: "#FFC735"
        font.pixelSize: 11
        anchors.centerIn: parent
        verticalAlignment: Text.AlignVCenter
    }
}
