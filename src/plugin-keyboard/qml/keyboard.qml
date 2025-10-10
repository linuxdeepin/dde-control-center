// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import org.deepin.dcc 1.0

DccObject {
    id: keyboard
    DccObject {
        name: "keyboard"
        parentName: "device"
        displayName: qsTr("Keyboard")
        description: qsTr("General Settings, input method, shortcuts")
        icon: "device_keyboard"
        weight: 40
    }

    visible: false
    DccDBusInterface {
        property var numLockState
        service: "org.deepin.dde.Keybinding1"
        path: "/org/deepin/dde/Keybinding1"
        inter: "org.deepin.dde.Keybinding1"
        connection: DccDBusInterface.SessionBus
        onNumLockStateChanged: keyboard.visible = true
    }
}
