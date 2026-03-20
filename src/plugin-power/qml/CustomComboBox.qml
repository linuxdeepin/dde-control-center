// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

D.ComboBox {
    id: control
    flat: true
    property string visibleRole
    property string enableRole

    delegate: D.MenuItem {
        id: menuItem
        useIndicatorPadding: true
        readonly property var safeModelData: typeof modelData !== "undefined" ? modelData : null
        
        text: control.textRole ? (Array.isArray(control.model) ? safeModelData[control.textRole] : model[control.textRole]) : (safeModelData !== null ? safeModelData : "")
        icon.name: (control.iconNameRole && model[control.iconNameRole] !== undefined) ? model[control.iconNameRole] : null
        highlighted: control.highlightedIndex === index
        hoverEnabled: control.hoverEnabled
        autoExclusive: true
        checked: control.currentIndex === index
        enabled: (control.enableRole && model[control.enableRole] !== undefined) ? model[control.enableRole] : true
        visible: (control.visibleRole && model[control.visibleRole] !== undefined) ? model[control.visibleRole] : true
        implicitHeight: visible ? DS.Style.control.implicitHeight(menuItem) : 0

        readonly property real availableTextWidth: {
            return contentItem.width - contentItem.leftPadding - contentItem.rightPadding
        }

        FontMetrics {
            id: fontMetrics
            font: menuItem.font
        }

        HoverHandler { id: hoverHandler }

        ToolTip.visible: hoverHandler.hovered && fontMetrics.advanceWidth(menuItem.text) > menuItem.availableTextWidth
        ToolTip.text: menuItem.text
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
