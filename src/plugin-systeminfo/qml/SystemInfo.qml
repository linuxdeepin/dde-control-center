// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.0
import QtQuick.Controls 2.0

import org.deepin.dtk 1.0 as D

import org.deepin.dcc 1.0

DccTitleObject {
    id: root
    name: "auxiliaryInfo"
    parentName: "system"
    displayName: qsTr("Auxiliary Information")
    weight: 1000
    onParentItemChanged: {
        if (parentItem) {
            parentItem.topPadding = 10
        }
    }

    DccObject {
        name: "systemInfo"
        parentName: "system"
        displayName: qsTr("About This PC")
        description: qsTr("System version, device information")
        icon: "about"
        weight: 1010
    }
    DccObject {
        name: "versionProtocol"
        parentName: "system"
        displayName: qsTr("Open Source Software Notice")
        description: qsTr("View the notice of open source software")
        icon: "software_declaration"
        weight: 1020
    }
    DccObject {
        name: "userExperienceProgram"
        parentName: "system"
        displayName: qsTr("User Experience Program")
        description: qsTr("Join the user experience program to help improve the product")
        icon: "user_experience_plan"
        visible: !DccApp.isServerSystem() && !DccApp.isCommunitySystem() && DccApp.isDeepin()
        weight: 1030
    }
    DccObject {
        name: "userLicense"
        parentName: "system"
        displayName: qsTr("End User License Agreement")
        description: qsTr("View the end  user license agreement")
        icon: "user_license_agreement"
        visible: true
        weight: 1040
    }
    DccObject {
        name: "privacyPolicy"
        parentName: "system"
        displayName: qsTr("Privacy Policy")
        description: qsTr("View information about privacy policy")
        icon: "privacy_policy"
        weight: 1050
    }
}
