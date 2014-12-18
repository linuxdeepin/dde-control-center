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

import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    id: brightnessRec

    width: parent.width
    height: visible ? childrenRect.height + 10 : 0

    property string pOutputObjName: ""
    property var brightnessValues: displayId.brightness
    property int itemIndex: 0

    onBrightnessValuesChanged: {
        if(!oneBrightnessSlider.pressedFlag && pOutputObjName != "") {
            oneBrightnessSlider.setValue(brightnessValues[pOutputObjName])
        }
    }

    Rectangle {
        width: parent.width - 30
        height: 2
        color: dconstants.bgColor
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        visible: itemIndex != 0
    }

    DssH2 {
        id:monitorName
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 30
        color: dconstants.fgDarkColor
        text: dsTr("Monitor") + " " + pOutputObjName
    }

    DSliderEnhanced {
        id: oneBrightnessSlider
        width: parent.width - 40
        height: 28
        anchors.top: monitorName.bottom
        anchors.topMargin: 5
        anchors.horizontalCenter: parent.horizontalCenter

        min: 0
        max: 1.0
        init: pOutputObjName ? brightnessValues[pOutputObjName] : max
        valueDisplayVisible: false

        onValueChanged:{
            if(pOutputObjName && value >= 0 && value <= 1 && pressedFlag){
                displayId.SetBrightness(pOutputObjName, value)
            }
        }
    }
}
