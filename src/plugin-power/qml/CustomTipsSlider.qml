// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick

import org.deepin.dtk as D
import org.deepin.dtk.style as DS
import org.deepin.dcc

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
