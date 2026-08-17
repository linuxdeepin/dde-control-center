// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import org.deepin.dtk as D

D.ActionButton {
    property real size: 16
    checked: true
    activeFocusOnTab: false
    icon {
        width: size
        height: size
        name: checked ? "item_checked" : "item_unchecked"
    }
}
