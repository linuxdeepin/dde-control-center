// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15

ItemDelegate {
    id: root
    property alias isGroup: background.isGroup
    property alias separatorVisible: background.separatorVisible
    width: parent.width
    backgroundVisible: false
    checkable: false
    implicitHeight: contentItem.height
    padding: 0

    contentItem: model.item.getSectionItem()
    background: DccListViewBackground {
        id: background
        separatorVisible: false
    }
}
