// SPDX-FileCopyrightText: 2024-2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import org.deepin.dcc 1.0

DccObject {
    name: "defaultapp"
    parentName: "system"
    displayName: qsTr("Default App")
    description: qsTr("Set the default application for opening various types of files")
    icon: "default_program"
    weight: 50
}
