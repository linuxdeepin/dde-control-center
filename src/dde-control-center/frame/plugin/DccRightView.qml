// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.2

ListView {
    focus: true
    model: DccModel {
        id: dccModel
        root: dccObj
    }
    //禁止listview控件回弹，解决白色空白出现
    boundsBehavior: Flickable.StopAtBounds
    delegate: DelegateChooser {
        role: "pageType"
        DelegateChoice {
            roleValue: DccObject.Menu
            delegate: DccMenuComponent {}
        }
        DelegateChoice {
            roleValue: DccObject.Editor
            delegate: DccEditorComponent {}
        }
        DelegateChoice {
            roleValue: DccObject.Item
            delegate: DccItemComponent {}
        }
    }
}
