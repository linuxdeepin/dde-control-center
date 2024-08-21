// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ItemDelegate {
    id: root
    property alias isGroup: background.isGroup
    property alias separatorVisible: background.separatorVisible
    property var item: model.item

    Layout.fillWidth: true
    backgroundVisible: false
    enabled: model.item.enabledToApp
    checkable: false
    implicitHeight: contentItem.height
    padding: 0

    contentItem: model.item.getSectionItem()
    background: DccListViewBackground {
        id: background
        separatorVisible: false
    }
}
