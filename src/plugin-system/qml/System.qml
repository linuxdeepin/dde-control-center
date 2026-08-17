// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls

import org.deepin.dtk as D

import org.deepin.dcc

DccObject {
    id: root
    name: "system"
    parentName: "root"
    displayName: qsTr("System")
    icon: "commoninfo"
    weight: 10

    DccTitleObject {
        name: "common"
        parentName: "system"
        displayName: qsTr("Common settings")
        weight: 5
        onParentItemChanged: {
            if (parentItem) {
                parentItem.topPadding = 10
            }
        }
    }
}
