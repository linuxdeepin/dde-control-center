// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import Qt.labs.qmlmodels 1.2

import org.deepin.dtk 1.0 as D

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
            id: repeater
            model: DccModel {
                root: dccObj
            }

            delegate: DelegateChooser {
                role: "pageType"
                DelegateChoice {
                    roleValue: DccObject.Menu
                    delegate: DccMenuItem {
                        separatorVisible: root.isGroup
                        backgroundVisible: true
                        Layout.fillWidth: true
                    }
                }
                DelegateChoice {
                    roleValue: DccObject.Editor
                    delegate: DccEditorItem {
                        topInset: root.isGroup ? 0 : 5
                        bottomInset: root.isGroup ? 0 : 5
                        separatorVisible: root.isGroup
                        backgroundVisible: root.isGroup || model.item.hasBackground
                        Layout.fillWidth: true
                        corners: root.isGroup ? getCornersForBackground(index, repeater.count) : D.RoundRectangle.TopLeftCorner | D.RoundRectangle.TopRightCorner | D.RoundRectangle.BottomLeftCorner | D.RoundRectangle.BottomRightCorner
                    }
                }
                DelegateChoice {
                    roleValue: DccObject.Item
                    delegate: DccItem {
                        topInset: root.isGroup ? 0 : 5
                        bottomInset: root.isGroup ? 0 : 5
                        separatorVisible: root.isGroup
                        backgroundVisible: root.isGroup || model.item.hasBackground
                        Layout.fillWidth: true
                        corners: root.isGroup ? getCornersForBackground(index, repeater.count) : D.RoundRectangle.TopLeftCorner | D.RoundRectangle.TopRightCorner | D.RoundRectangle.BottomLeftCorner | D.RoundRectangle.BottomRightCorner
                    }
                }
            }
        }
    }
}
