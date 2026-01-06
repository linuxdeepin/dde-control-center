// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import org.deepin.dcc 1.0

// 该文件中不能使用dccData,根对象为DccObject
DccObject {
    id: root
    name: "example"
    parentName: "root"
    displayName: qsTr("Example")
    icon: "dcc_example"
    weight: 1000

    visible: false // 控制模块显示
    DccDBusInterface {
        property var windowRadius
        service: "org.deepin.dde.Appearance1"
        path: "/org/deepin/dde/Appearance1"
        inter: "org.deepin.dde.Appearance1"
        connection: DccDBusInterface.SessionBus
        onWindowRadiusChanged: {
            root.visible = windowRadius > 0
        }
    }
}
