// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc 1.0

D.TipsSlider {
    id: slider
    property var dataMap
    slider.handleType: D.Slider.HandleType.ArrowBottom
    slider.from: 0
    slider.to: dataMap.length - 1
    slider.live: true
    slider.stepSize: 1
    slider.snapMode: D.Slider.SnapAlways
    Loader {
        Repeater {
            model: slider.dataMap.length
            D.SliderTipItem {
                // TODO need to modify
                parent: slider.children[1]
                text: slider.dataMap[index].text
                highlight: slider.slider.value === index
            }
        }
    }
}
