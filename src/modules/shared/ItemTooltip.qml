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
    width: 0
    state: "hideTip"
    clip: true

    property alias textItem: tipText
    property alias tooltip: tipText.text
    property int animationDuration: 500
    property int autoHideInterval: -1
    property int delayShowInterval: -1
    property int leftMargin: 30

    signal show(int tipLength)
    signal hide()

    states: [
        State {
            name: "showTip"
            PropertyChanges { target: tipText; x: leftMargin;}
        },
        State {
            name: "hideTip"
            PropertyChanges { target: tipText; x: buttonToolTip.width;}

        }
    ]

    transitions:[
        Transition {
            from: "showTip"
            to: "hideTip"
            ParallelAnimation {
                NumberAnimation {target: tipText;property: "x";duration: animationDuration;easing.type: Easing.OutCubic}
            }
        },
        Transition {
            from: "hideTip"
            to: "showTip"
            ParallelAnimation {
                NumberAnimation {target: tipText;property: "x"; duration: animationDuration; easing.type: Easing.OutCubic}
            }
        }
    ]

    function showTip(){
        delayShowTimer.start()
    }

    function hideTip(){
        buttonToolTip.state = "hideTip"
        delayShowTimer.stop()
        buttonToolTip.hide()
    }

    function hideTipImmediately(){
        tipText.x = buttonToolTip.width
        buttonToolTip.state = "hideTip"
        delayShowTimer.stop()
        buttonToolTip.hide()
    }

    Timer {
        id:delayShowTimer
        repeat: false
        running: false
        interval: delayShowInterval > 0 ? delayShowInterval : 0
        onTriggered: {
            buttonToolTip.show(tipText.implicitWidth)
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
        clip: true
        wrapMode: Text.Wrap
        width: parent.width - leftMargin
        height: parent.height
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
    }
}
