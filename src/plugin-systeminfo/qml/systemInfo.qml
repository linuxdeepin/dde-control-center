// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.0
import QtQuick.Controls 2.0

import org.deepin.dtk 1.0 as D

import org.deepin.dcc 1.0

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
