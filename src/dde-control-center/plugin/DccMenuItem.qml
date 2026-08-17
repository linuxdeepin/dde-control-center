// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls

import org.deepin.dtk as D

DccEditorItem {
    topInset: 5
    leftPadding: 14
    rightPadding: 10
    bottomInset: 5
    topPadding: topInset
    bottomPadding: bottomInset
    activeFocusOnTab: false
    rightItem: D.IconLabel {
        icon {
            name: "arrow_ordinary_right"
            palette: D.DTK.makeIconPalette(palette)
        }
    }
    onClicked: DccApp.showPage(model.item)
}
