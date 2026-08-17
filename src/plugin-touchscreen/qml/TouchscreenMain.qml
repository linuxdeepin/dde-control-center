// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Window
import QtQuick.Controls

import org.deepin.dcc
import org.deepin.dtk as D

DccObject {
    DccObject {
        name: "touchscreen"
        parentName: "touchscreen"
        displayName: qsTr("Common")
        weight: 10
        pageType: DccObject.Item
        page: DccGroupView {
            spacing: 5
            isGroup: false
            height: implicitHeight + 20
        }
        TouchScreen {}
    }
}
