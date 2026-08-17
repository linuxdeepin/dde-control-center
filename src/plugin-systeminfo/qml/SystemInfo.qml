// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls

import org.deepin.dtk as D

import org.deepin.dcc

DccTitleObject {
    id: root
    name: "auxiliaryInfo"
    parentName: "system"
    displayName: qsTr("Auxiliary Information")
    weight: 1000
    onParentItemChanged: {
        if (parentItem) {
            parentItem.topPadding = 10
        }
    }
}
