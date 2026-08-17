// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick
import QtQuick.Controls

import org.deepin.dcc
import QtQuick.Layouts

DccObject {
    DccObject {
        name: "bootMenu"
        parentName: "system"
        displayName: qsTr("Boot Menu")
        description: qsTr("Manage your boot menu")
        icon: "meau"
        weight: 80
        BootPage {}
    }
    DccObject {
        name: "developerMode"
        parentName: "system"
        displayName: qsTr("Developer Options")
        description: !dccData.mode().isCommunitySystem() ? qsTr("Developer root permission management") : qsTr("Developer debugging options")
        icon: "developer"
        weight: 90
        DevelopModePage {}
    }
}
