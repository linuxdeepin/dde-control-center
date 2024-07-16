// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import Qt.labs.qmlmodels 1.2

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

ListView {
    focus: true
    implicitHeight: contentHeight
    // anchors.fill: parent
    clip: true
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
            delegate: DccMenuComponent {
                isGroup: true
                separatorVisible: true
                backgroundVisible: true
            }
        }
        DelegateChoice {
            roleValue: DccObject.Editor
            delegate: DccEditorComponent {
                isGroup: true
                separatorVisible: true
                backgroundVisible: true
            }
        }
        DelegateChoice {
            roleValue: DccObject.Item
            delegate: DccItemComponent {
                isGroup: true
                separatorVisible: true
                backgroundVisible: true
            }
        }
    }
}
