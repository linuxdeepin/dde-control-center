// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import org.deepin.dcc 1.0

DccObject {
    id: root
    name: "touchscreen"
    parentName: "device"
    displayName: qsTr("Touchscreen")
    description: qsTr("Configuring Touchscreen")
    icon: "dcc_nav_touchscreen"
    visible: false
    weight: 60
    DccDBusInterface {
        service: "org.deepin.dde.Display1"
        path: "/org/deepin/dde/Display1"
        inter: "org.deepin.dde.Display1"
        connection: DccDBusInterface.SessionBus
        monitorProperties: ["TouchscreensV2"]
        onPropertyChanged: function (properties) {
            if (properties.hasOwnProperty("TouchscreensV2")) {
                root.visible = properties["TouchscreensV2"].length !== 0
            }
        }
    }
}
