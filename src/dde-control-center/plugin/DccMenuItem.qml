// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15

import org.deepin.dtk 1.0 as D

DccEditorItem {
    id: control
    topInset: 5
    leftPadding: 14
    rightPadding: 10
    bottomInset: 5
    topPadding: topInset
    bottomPadding: bottomInset
    activeFocusOnTab: false
    rightItem: D.DciIcon {
        palette: D.DTK.makeIconPalette(control.palette)
        mode: control.D.ColorSelector.controlState
        theme: control.D.ColorSelector.controlTheme
        name: "arrow_ordinary_right"
    }
    onClicked: DccApp.showPage(model.item)
}
