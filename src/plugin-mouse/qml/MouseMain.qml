// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Window
import QtQuick.Controls

import org.deepin.dcc
import org.deepin.dtk as D

DccObject {
    Connections {
        target: DccApp
        function onCurrentObjectsChanged(objects) {
            for (var i = 0; i < objects.length; i++) {
                if (objects[i].name === "MouseAndTouchpad") {
                    dccData.refreshMouse()
                    break
                }
            }
        }
    }
    DccObject {
        name: "MouseAndTouchpadCommon"
        parentName: "MouseAndTouchpad"
        displayName: qsTr("Common")
        weight: 10
        pageType: DccObject.Item
        page: DccGroupView {
            spacing: 0
            isGroup: false
        }
        Common {}
    }
    DccObject {
        name: "MouseAndTouchpadMouse"
        parentName: "MouseAndTouchpad"
        displayName: qsTr("Mouse")
        icon: "mouse_trackpad_mouse"
        weight: 100
        page: DccRightView {
            spacing: 5
        }
        MousePage {}
    }
    DccObject {
        name: "Touchpad"
        parentName: "MouseAndTouchpad"
        displayName: qsTr("Touchpad")
        icon: "mouse_trackpad_trackpad"
        visible: dccData.tpadExist
        weight: 200
        page: DccRightView {
            spacing: 0
        }
        Touchpad {}
    }
}
