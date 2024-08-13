// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import Qt.labs.qmlmodels 1.2

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

Rectangle {
    id: root
    property bool isGroup: true
    property alias spacing: layoutView.spacing

    color: "transparent"
    implicitHeight: layoutView.height
    Layout.fillWidth: true
    ColumnLayout {
        id: layoutView
        width: parent.width
        spacing: 0
        Repeater {
            model: DccModel {
                root: dccObj
            }

            delegate: DelegateChooser {
                role: "pageType"
                DelegateChoice {
                    roleValue: DccObject.Menu
                    delegate: DccMenuComponent {
                        isGroup: root.isGroup
                        separatorVisible: root.isGroup
                        backgroundVisible: true
                        Layout.fillWidth: true
                    }
                }
                DelegateChoice {
                    roleValue: DccObject.Editor
                    delegate: DccEditorComponent {
                        isGroup: root.isGroup
                        separatorVisible: root.isGroup
                        backgroundVisible: root.isGroup || model.item.hasBackground
                        Layout.fillWidth: true
                    }
                }
                DelegateChoice {
                    roleValue: DccObject.Item
                    delegate: DccItemComponent {
                        isGroup: root.isGroup
                        separatorVisible: root.isGroup
                        backgroundVisible: root.isGroup || model.item.hasBackground
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }
}
