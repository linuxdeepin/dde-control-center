// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D

D.ItemDelegate {
    id: root
    property alias separatorVisible: background.separatorVisible
    property alias backgroundType: background.backgroundType
    property var item: model.item

    backgroundVisible: backgroundType & 0x01
    enabled: model.item.enabledToApp
    hoverEnabled: true
    checkable: false
    topPadding: topInset
    bottomPadding: bottomInset
    implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding
    padding: 0
    font: D.DTK.fontManager.t6

    contentItem: model.item.getSectionItem(this)
    background: DccItemBackground {
        id: background
        separatorVisible: false
    }
    Component.onCompleted: {
        if (contentItem.objectName === "noPadding") {
            leftPadding = 0
            rightPadding = 0
        }
        model.item.parentItem = root
    }
}
