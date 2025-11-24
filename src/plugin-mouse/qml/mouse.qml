// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import org.deepin.dcc 1.0

DccObject {
    id: mouse
    name:"MouseAndTouchpad"
    parentName:"device"
    displayName: qsTr("Mouse and Touchpad")
    description: qsTr("Common、Mouse、Touchpad")
    icon:"device_mouse"
    weight: 30

    page: DccRightView {
        spacing: -4
    }

    visible: !DccApp.isTreeland()
}
