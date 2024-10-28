// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import org.deepin.dcc 1.0
import QtQuick.Layouts
DccObject {
    // 账户头像
    DccObject {
        name: "userAvatars"
        parentName: "acounts"
        weight: 10
        pageType: DccObject.Item
        page: RowLayout {

        }
    }

    // 账户信息
    DccObject {
        name: "acountInfos"
        parentName: "acounts"
        displayName: qsTr("Acount Informations")
        description: qsTr("Acount name, acount fullname, account type")
        weight: 20
        pageType: DccObject.Item
        // page: DccGroupView {}
    }

    // 登陆设置
    DccObject {
        name: "acountSettings"
        parentName: "acounts"
        displayName: qsTr("Acount Settings")
        description: qsTr("Auto longin, login without password")
        weight: 30
        pageType: DccObject.Item
        // page: DccGroupView {}
    }

    // 登陆方式
    DccObject {
        name: "loginMethod"
        parentName: "acounts"
        displayName: qsTr("Login method")
        description: qsTr("Password, wechat, biometric authentication, security key")
        weight: 40
        pageType: DccObject.Item
        // page: DccGroupView {}
    }

    // 用户组
    DccObject {
        name: "userGroups"
        parentName: "acounts"
        displayName: qsTr("User groups")
        weight: 50
        pageType: DccObject.Item
        // page: DccGroupView {}
    }
    // 用户组
    DccObject {
        name: "otherAcounts"
        parentName: "acounts"
        displayName: qsTr("Other accounts")
        weight: 60
        pageType: DccObject.Item
        // page: DccGroupView {}
    }
}
