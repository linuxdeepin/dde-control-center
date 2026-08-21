// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

import QtQuick
import org.deepin.dtk 1.0
import org.deepin.dtk.style 1.0 as DS


// AutoSizingComboBox — ComboBox 封装，自动适配下拉面板宽度
ComboBox {
    id: control

    property real _popupContentWidth: 0

    TextMetrics {
        id: _textMetrics
        font: control.font
    }

    /**
     * 文本像素宽度之外的额外填充总和（最坏情况：当前选中项 checked=true 时 indicator 可见）
     *
     * T.MenuItem 内容布局（对 checked 项，indicator 显式时）：
     *   ┌──────────────────────────────────────────────────────────┐
     *   │ leftPadding(6) │ indicator(14) │ spacing(6) │ contentItem(IconLabel) │ rightPadding(6) │
     *   └──────────────────────────────────────────────────────────┘
     *
     * Style 常量来源（dtkdeclarative/qt6/src/qml/FlowStyle.qml）：
     *   DS.Style.popup.margin             = 10  Popup contentItem 边距
     *   DS.Style.control.padding          = 6   MenuItem 外 padding
     *   DS.Style.control.spacing          = 6   Indicator 与 contentItem 间距
     *   DS.Style.menu.item.iconSize.width = 14  Indicator（checkmark）宽度
     *   DS.Style.menu.item.contentPadding = 30  IconLabel 左 padding
     */

    readonly property real _textExtraPadding: DS.Style.menu.item.contentPadding       // 30
                                             + DS.Style.control.padding * 2            // 12
                                             + DS.Style.popup.margin * 2               // 20
                                             + DS.Style.menu.item.iconSize.width       // 14  (indicator)
                                             + DS.Style.control.spacing                // 6   (indicator spacing)
                                                                                       // = 82

    function _updatePopupWidth() {
        var m = model
        if (!m) {
            _popupContentWidth = 0
            return
        }

        var count = typeof m.count === 'number' ? m.count
                  : (typeof m.length === 'number' ? m.length : 0)
        if (count <= 0) {
            _popupContentWidth = 0
            return
        }

        var usesGet = typeof m.get === 'function'

        var maxWidth = 0
        for (var i = 0; i < count; ++i) {
            var item = usesGet ? m.get(i) : m[i]
            if (!item) {
                continue
            }

            var text = textRole ? item[textRole] : item.text
            if (text === undefined || text === "") {
                continue
            }

            _textMetrics.text = String(text)
            var w = _textMetrics.width + _textExtraPadding
            if (w > maxWidth) {
                maxWidth = w
            }
        }
        _popupContentWidth = maxWidth
    }

    onModelChanged: _updatePopupWidth()
    onFontChanged: _updatePopupWidth()
    Component.onCompleted: _updatePopupWidth()

    popup.implicitWidth: Math.max(_popupContentWidth, width)
}
