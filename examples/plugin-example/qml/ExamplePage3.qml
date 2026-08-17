// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.deepin.dcc

DccSettingsObject {
    id: root
    DccRepeater {
        model: 13
        delegate: DccObject {
            name: "item" + (index + 1)
            parentName: root.bodyUrl
            displayName: qsTr("Item") + (index + 1)
            weight: 30 + index
            backgroundType: DccObject.Normal
            pageType: DccObject.Editor
            page: Switch {}
        }
    }
    DccObject {
        name: "delete"
        parentName: root.footerUrl
        weight: 10
        pageType: DccObject.Item
        page: Button {
            text: qsTr("Delete")
        }
    }
}
