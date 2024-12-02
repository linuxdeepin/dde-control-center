// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.3

import org.deepin.dcc 1.0

DccObject {
    DccObject {
        name: "KeyboardCommon"
        parentName: "keyboard"
        displayName: qsTr("Common")
        weight: 10
        pageType: DccObject.Item
        page: DccGroupView {
            spacing: 5
            isGroup: false
            height: implicitHeight + 20
        }
        Common {}
    }
    DccObject {
        name: "KeyboardLayout"
        parentName: "keyboard"
        displayName: qsTr("Keyboard layout")
        icon: "dcc_nav_keyboard"
        weight: 100
        page: DccRightView {
            spacing: 5
        }
        KeyboardLayout {}
    }

    // 101~299 for InputMethod

    Shortcuts {
        weight: 300
    }
}
