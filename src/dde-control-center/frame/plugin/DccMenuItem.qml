// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15

import org.deepin.dtk 1.0 as D

DccEditorItem {
    topInset: 5
    leftPadding: 12
    rightPadding: 10
    bottomInset: 5
    implicitHeight: 58
    topPadding: topInset
    bottomPadding: bottomInset

    rightItem: D.IconLabel {
        icon.name: "arrow_ordinary_right"
    }
    onClicked: {
        model.item.trigger()
        console.log(model.item.name, model.display, model.item.icon)
    }
}
