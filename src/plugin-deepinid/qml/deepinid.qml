// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

import org.deepin.dcc 1.0

DccObject {
    id: root
    name: "deepinid"
    parentName: "root"
    displayName: DccApp.uosEdition() === DccApp.UosCommunity ? qsTr("Deepin ID") : qsTr("UOS ID")
    description: DccApp.uosEdition() === DccApp.UosCommunity ? qsTr("this is DeepinID") : qsTr("this is UOSID")
    icon: "deepinid"
    weight: 20
    
    visible: false
    DccDBusInterface {
        property var isLogin
        service: "com.deepin.deepinid"
        path: "/com/deepin/deepinid"
        inter: "com.deepin.deepinid"
        connection: DccDBusInterface.SessionBus
        onIsLoginChanged: {
            root.visible = true
        }
    }
}
