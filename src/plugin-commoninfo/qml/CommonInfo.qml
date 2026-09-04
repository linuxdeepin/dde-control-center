// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15

import org.deepin.dcc 1.0

DccObject {
    name: "bootMenu"
    parentName: "system"
    displayName: qsTr("Boot Menu")
    description: qsTr("Manage your boot menu")
    icon: "meau"
    weight: 80
    DccObject {
        name: "developerMode"
        parentName: "system"
        displayName: qsTr("Developer Options")
        description: !dccData.mode().isCommunitySystem() ? qsTr("Developer root permission management") : qsTr("Developer debugging options")
        icon: "developer"
        weight: 90
    }
}
