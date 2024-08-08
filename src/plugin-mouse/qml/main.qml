// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    DccObject {
        name:"MouseAndTouchpadCommon"
        parentName:"MouseAndTouchpad"
        displayName: qsTr("Common")
        weight: 10
        pageType: DccObject.Item
        // page:
        page: Common{}
    }
    DccObject {
        name:"Mouse"
        parentName:"MouseAndTouchpad"
        displayName: qsTr("Mouse")
        // description: qsTr("控制系统声音效果")
        icon:"dcc_nav_mouse"
        weight: 20
        page: Mouse{}
    }
    DccObject {
        name:"Touchpad"
        parentName:"MouseAndTouchpad"
        displayName: qsTr("Touchpad")
        icon:"dcc_nav_mouse"
        weight: 20
        page: Touchpad{}
    }
}
