// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import org.deepin.dcc 1.0

DccObject {
    id: root
    name: "touchscreen"
    parentName: "device"
    displayName: qsTr("Touchscreen")
    description: qsTr("Configuring Touchscreen")
    icon: "device_touchscreen"
    visible: false
    weight: 50
    DccDBusInterface {
        property var touchscreensV2
        service: "org.deepin.dde.Display1"
        path: "/org/deepin/dde/Display1"
        inter: "org.deepin.dde.Display1"
        connection: DccDBusInterface.SessionBus
        onTouchscreensV2Changed: {
            root.visible = touchscreensV2.length !== 0
        }
    }
}
