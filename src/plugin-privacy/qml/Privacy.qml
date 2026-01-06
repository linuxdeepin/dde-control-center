// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    name: "privacy"
    parentName: "root"
    displayName: qsTr("Privacy and Security")
    description: qsTr("Camera, folder permissions")
    icon: "privacy"
    weight: 80
    visible: typeof D.SysInfo !== 'undefined' && DccApp.productType() === D.SysInfo.Uos
}
