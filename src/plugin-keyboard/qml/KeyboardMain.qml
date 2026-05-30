// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.3

import org.deepin.dcc 1.0

DccObject {
    Connections {
        target: DccApp
        function onCurrentObjectsChanged(objects) {
            for (var i = 0; i < objects.length; i++) {
                if (objects[i].name === "keyboard") {
                    dccData.refreshKeyboard()
                    break
                }
            }
        }
    }
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
    // 101~299 for InputMethod

    Shortcuts {
        weight: 300
    }
}
