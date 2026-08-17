// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import org.deepin.dcc
import org.deepin.dtk as D
import org.deepin.dtk.style as DS

D.ComboBox {
    id: control
    flat: true
    textRole: "text"
    valueRole: "value"

    delegate: D.MenuItem {
        id: menuItem
        useIndicatorPadding: true
        width: control.width
        text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData
        highlighted: control.isInteractingWithContent ? control.highlightedIndex === index : false
        hoverEnabled: control.hoverEnabled
        autoExclusive: true
        checked: control.currentIndex === index
        implicitHeight: DS.Style.control.implicitHeight(menuItem)

        readonly property real availableTextWidth: contentItem ? contentItem.width : (width - leftPadding - rightPadding)

        FontMetrics {
            id: fontMetrics
            font: menuItem.font
        }

        D.ToolTip {
            visible: menuItem.hovered && fontMetrics.advanceWidth(menuItem.text) > menuItem.availableTextWidth
            text: menuItem.text
            delay: 500
        }
    }

    // To replace function: indexOfValue
    function indexByValue(value) {
        for (var i = 0; i < model.count; i++) {
            if (model.get(i).value === value) {
                return i;
            }
        }
        return -1;
    }
}
