// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    DccObject {
        name: "common"
        parentName: "powerManage"
        displayName: qsTr("通用")
        icon: "power"
        weight: 10
        page: DccRightView {
            spacing: 5
        }
        CommonPage {}
    }
    DccObject {
        name: "power"
        parentName: "powerManage"
        displayName: qsTr("电源")
        icon: "power"
        weight: 100
        page: DccRightView {
            spacing: 5
        }
        PowerPage {}
    }
    DccObject {
        name: "battery"
        parentName: "powerManage"
        displayName: qsTr("电池")
        icon: "power"
        weight: 200
        page: DccRightView {
            spacing: 5
        }
        BatteryPage {}
    }
}
