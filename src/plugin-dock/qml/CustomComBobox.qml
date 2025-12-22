//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

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
        highlighted: control.highlightedIndex === index
        hoverEnabled: control.hoverEnabled
        autoExclusive: true
        checked: control.currentIndex === index
        implicitHeight: DS.Style.control.implicitHeight(menuItem)

        readonly property real availableTextWidth: {
            if (!contentItem)
                return width - leftPadding - rightPadding
            return contentItem.width - contentItem.leftPadding - contentItem.rightPadding
        }

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
