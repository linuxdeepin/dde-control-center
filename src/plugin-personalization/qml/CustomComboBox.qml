// SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

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
        text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : (model[control.textRole] === undefined ? modelData[control.textRole] : model[control.textRole])) : modelData
        icon.name: control.iconNameRole ? ((model[control.iconNameRole] !== undefined ? model[control.iconNameRole] : modelData[control.iconNameRole])) : null
        highlighted: control.highlightedIndex === index
        hoverEnabled: control.hoverEnabled
        autoExclusive: true
        checked: control.currentIndex === index
        enabled: control.enableRole ? ((model[control.enableRole] !== undefined ? model[control.enableRole] : modelData[control.enableRole])) : true
        visible: control.visibleRole ? ((model[control.visibleRole] !== undefined ? model[control.visibleRole] : modelData[control.visibleRole])) : true
        implicitHeight: visible ? DS.Style.control.implicitHeight(menuItem) : 0
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
