// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import org.deepin.dcc 1.0

DccObject {
    id: power
    name: "power"
    parentName: "root"
    displayName: qsTr("Power")
    description: qsTr("Power saving settings, screen and suspend")
    icon: "power"
    weight: 50

    visible: false
    DccDBusInterface {
        property var onBattery
        service: "org.deepin.dde.Power1"
        path: "/org/deepin/dde/Power1"
        inter: "org.deepin.dde.Power1"
        connection: DccDBusInterface.SessionBus
        onOnBatteryChanged: power.visible = true
    }
}
