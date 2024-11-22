// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import org.deepin.dcc 1.0

DccObject {
    id: root
    name: "sound"
    parentName: "system"
    displayName: qsTr("sound")
    icon: "audio"
    visible: false
    weight: 20

    DccDBusInterface {
        property var sleepLock
        service: "org.deepin.dde.Power1"
        path: "/org/deepin/dde/Power1"
        inter: "org.deepin.dde.Power1"
        connection: DccDBusInterface.SessionBus
        onSleepLockChanged: {
            root.visible = true
        }
    }
}
