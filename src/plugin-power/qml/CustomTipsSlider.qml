// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc 1.0

D.TipsSlider {
    id: slider
    property var dataMap: []
    property var __tickItems: []
    slider.handleType: D.Slider.HandleType.ArrowBottom
    slider.from: 0
    slider.to: Math.max(0, (slider.dataMap || []).length - 1)
    slider.live: true
    slider.stepSize: 1
    slider.snapMode: D.Slider.SnapAlways

    Component {
        id: tickItem
        D.SliderTipItem {
            property int tickIndex: -1
            text: slider.dataMap && slider.dataMap[tickIndex] ? slider.dataMap[tickIndex].text : ""
            highlight: slider.slider.value === tickIndex
        }
    }

    function findGrid() {
        for (var i = 0; i < slider.children.length; ++i) {
            if (slider.children[i] instanceof Grid)
                return slider.children[i]
        }
        return null
    }

    function syncTicks() {
        var count = slider.dataMap ? slider.dataMap.length : 0
        if (__tickItems.length === count)
            return

        for (var i = 0; i < __tickItems.length; ++i) {
            if (__tickItems[i]) {
                __tickItems[i].parent = null
                __tickItems[i].destroy()
            }
        }
        __tickItems = []

        var grid = slider.findGrid()
        if (!grid) {
            console.warn("CustomTipsSlider: ticksGrid not found")
            return
        }

        for (var j = 0; j < count; ++j)
            __tickItems.push(tickItem.createObject(grid, { tickIndex: j }))
    }

    onDataMapChanged: slider.syncTicks()
    Component.onCompleted: slider.syncTicks()
}
