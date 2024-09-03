// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.0

import org.deepin.dcc 1.0
import QtQuick.Layouts 1.15

DccObject {
    DccObject {
        name: "systemInfo"
        parentName: "system"
        displayName: qsTr("About This PC")
        description: qsTr("View information about this machine")
        icon: "system"
        weight: 1010
        NativeInfoPage{}
    }
    DccObject {
        name: "versionProtocol"
        parentName: "system"
        displayName: qsTr("Edition License")
        description: qsTr("View GCC related protocol statements")
        icon: "system"
        weight: 1020
        VersionProtocolPage{}
    }
    DccObject {
        name: "userExperienceProgram"
        parentName: "system"
        displayName: qsTr("User Experience Program")
        description: qsTr("Join the User Experience Program and help improve the product")
        icon: "system"
        weight: 1030
        UserExperienceProgramPage{}
    }
    DccObject {
        name: "userLicense"
        parentName: "system"
        displayName: qsTr("End User License Agreement")
        description: qsTr("View End User License Agreement")
        icon: "system"
        visible: true
        weight: 1040
        UserLicensePage{}
    }
    DccObject {
        name: "privacyPolicy"
        parentName: "system"
        displayName: qsTr("Privacy Policy")
        description: qsTr("View Privacy Policy")
        icon: "system"
        weight: 1050
        PrivacyPolicyPage{}
    }
}
