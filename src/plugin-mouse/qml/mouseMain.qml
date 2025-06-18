// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    DccObject {
        name: "MouseAndTouchpadCommon"
        parentName: "MouseAndTouchpad"
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
        name: "MouseAndTouchpadMouse"
        parentName: "MouseAndTouchpad"
        displayName: qsTr("Mouse")
        icon: "mouse_trackpad_mouse"
        weight: 100
        page: DccRightView {
            spacing: 5
        }
        Mouse {}
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
