// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

D.ComboBox {
    id: control
    flat: true
    property string visibleRole

    displayText: {
        if (currentIndex < 0 || currentIndex >= model.count) {
            return ""
        } else {
            return model[currentIndex][textRole]
        }
    }

    delegate: D.MenuItem {
        id: menuItem
        useIndicatorPadding: true
        width: parent.width
        text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : (model[control.textRole] === undefined ? modelData[control.textRole] : model[control.textRole])) : modelData
        font.family: text
        icon.name: (control.iconNameRole && model[control.iconNameRole] !== undefined) ? model[control.iconNameRole] : null
        highlighted: control.highlightedIndex === index
        hoverEnabled: control.hoverEnabled
        autoExclusive: true
        checked: control.currentIndex === index
        enabled: (control.enableRole && model[control.enableRole] !== undefined) ? model[control.enableRole] : true
        visible: (control.visibleRole && model[control.visibleRole] !== undefined) ? model[control.visibleRole] : true
        implicitHeight: visible ? DS.Style.control.implicitHeight(menuItem) : 0
    }
}
