// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import org.deepin.dcc 1.0

DccObject {
    id: root
    name: "wacom"
    parentName: "device"
    displayName: qsTr("wacom")
    description: qsTr("Configuring wacom")
    icon: "dcc_nav_wacom"
    visible: false
    weight: 60
    DccDBusInterface {
        property var exist
        service: "org.deepin.dde.InputDevices1"
        path: "/org/deepin/dde/InputDevice1/Wacom"
        inter: "org.deepin.dde.InputDevice1.Wacom"
        connection: DccDBusInterface.SessionBus
        onExistChanged: {
            root.visible = exist
        }
    }
}
