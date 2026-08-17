// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick

DccObject {
    id: root
    readonly property string bodyUrl: root.parentName + "/" + root.name + "/body"
    readonly property string footerUrl: root.parentName + "/" + root.name + "/footer"
    page: DccSettingsView {}
    DccObject {
        id: bodyObj
        name: "body"
        parentName: root.parentName + "/" + root.name
        weight: 10
        pageType: DccObject.Item
    }
    DccObject {
        id: footerObj
        name: "footer"
        parentName: root.parentName + "/" + root.name
        weight: 20
        pageType: DccObject.Item
    }
}
