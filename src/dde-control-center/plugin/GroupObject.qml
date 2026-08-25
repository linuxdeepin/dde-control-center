// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.0
import QtQuick.Controls 2.0

import org.deepin.dtk 1.0 as D

import org.deepin.dcc 1.0

DccObject {
    id: root

    DccObject {
        id: system
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

    DccObject {
        id: device
        name: "device"
        parentName: "root"
        displayName: qsTr("Bluetooth and Devices")
        icon: "hardware"
        weight: 40
    }
}
