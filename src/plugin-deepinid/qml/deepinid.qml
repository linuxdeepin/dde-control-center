// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.3
import org.deepin.dcc 1.0

DccObject {
    id: root
    name: "deepinid"
    parentName: "root"
    displayName: DccApp.uosEdition() === DccApp.UosCommunity ? qsTr("deepin ID") : qsTr("UOS ID")
    description: qsTr("Cloud services")
    icon: "deepinid"
    weight: 70

    page: Control {
        id: control
        width: parent ? parent.width : 10
        height: parent ? parent.height : 10
        contentItem: dccObj.children.length > 0 ? dccObj.children[0].getSectionItem(control) : null
    }

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
