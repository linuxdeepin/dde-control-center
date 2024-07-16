// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15

import org.deepin.dtk 1.0 as D

D.ItemDelegate {
    property alias isGroup: background.isGroup
    property alias separatorVisible: background.separatorVisible
    width: parent.width
    backgroundVisible: false
    checkable: false
    icon.name: model.icon
    text: model.display
    content: Control {
        contentItem: model.item.getSectionItem()
    }
    background: DccListViewBackground {
        id: background
        separatorVisible: false
    }
}
