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

DBaseLine {
    height: contentHeight
    leftMargin: contentLeftMargin
    leftLoader.sourceComponent: LeftTitle {
        text: dsTr("Left/Right Balance")
    }
    rightLoader.sourceComponent: DSliderEnhanced {
        id: lrBalanceSlider
        width: sliderWidth

        min: -1
        max: 1
        init: currentSink.balance
        completeColorVisible: false
        floatNumber: 2
        isBalance: true

        onValueChanged:{
            currentSink.SetBalance(value, false)
        }

        onValueConfirmed: {
            currentSink.SetBalance(value, true)
        }

        Component.onCompleted: {
            addRuler(-1, dsTr("Left"))
            addRuler(0, "")
            addRuler(1, dsTr("Right"))
        }

        Connections {
            target: currentSink

            onBalanceChanged: {
                if (!lrBalanceSlider.pressedFlag &&
                    Math.abs(currentSink.balance - lrBalanceSlider.value) >= 0.01) {
                    lrBalanceSlider.setValue(currentSink.balance)
                }
            }
        }

        Connections {
            target: soundModule

            onCurrentSinkChanged: {
                lrBalanceSlider.setValue(currentSink.balance)
            }
        }
    }
}
