// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.3

import org.deepin.dcc 1.0

DccObject {
    id: root
    page: Control {
        id: control
        anchors.fill: parent
        contentItem: dccObj.children.length > 0 ? dccObj.children[0].getSectionItem(control) : null
    }

    DeepinIDLogin {
        visible: !dccData.model.loginState
    }

    DeepinIDUserInfo {
        visible: dccData.model.loginState
    }
}
