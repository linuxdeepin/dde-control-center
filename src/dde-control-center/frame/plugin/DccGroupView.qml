// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import QtQml.Models //Delegatechoice for Qt >= 6.9
import Qt.labs.qmlmodels //DelegateChooser

import org.deepin.dtk 1.0 as D

Rectangle {
    id: root
    property bool isGroup: true
    property alias spacing: layoutView.spacing
    property var model: DccModel {
        root: dccObj
    }

    objectName: "noPadding"
    color: "transparent"
    implicitHeight: layoutView.height
    Layout.fillWidth: true
    ColumnLayout {
        id: layoutView
        width: parent.width
        spacing: 0
        Repeater {
            id: repeater
            model: root.model

            delegate: DelegateChooser {
                role: "pageType"
                DelegateChoice {
                    roleValue: DccObject.Menu
                    delegate: DccMenuItem {
                        topInset: root.isGroup ? 0 : 5
                        bottomInset: root.isGroup ? 0 : 5
                        separatorVisible: root.isGroup
                        backgroundType: model.item.backgroundType | 3
                        Layout.fillWidth: true
                        corners: root.isGroup ? getCornersForBackground(index, repeater.count) : D.RoundRectangle.TopLeftCorner | D.RoundRectangle.TopRightCorner | D.RoundRectangle.BottomLeftCorner | D.RoundRectangle.BottomRightCorner
                    }
                }
                DelegateChoice {
                    roleValue: DccObject.Editor
                    delegate: DccEditorItem {
                        topInset: root.isGroup ? 0 : 5
                        bottomInset: root.isGroup ? 0 : 5
                        separatorVisible: root.isGroup
                        backgroundType:model.item.backgroundType | (root.isGroup ? 1 : 0)
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
                        backgroundType: model.item.backgroundType | (root.isGroup ? 1 : 0)
                        Layout.fillWidth: true
                        corners: root.isGroup ? getCornersForBackground(index, repeater.count) : D.RoundRectangle.TopLeftCorner | D.RoundRectangle.TopRightCorner | D.RoundRectangle.BottomLeftCorner | D.RoundRectangle.BottomRightCorner
                    }
                }
                DelegateChoice {
                    roleValue: DccObject.MenuEditor
                    delegate: DccMenuEditorItem {
                        topInset: root.isGroup ? 0 : 5
                        bottomInset: root.isGroup ? 0 : 5
                        separatorVisible: root.isGroup
                        backgroundType: model.item.backgroundType | 3
                        Layout.fillWidth: true
                        corners: root.isGroup ? getCornersForBackground(index, repeater.count) : D.RoundRectangle.TopLeftCorner | D.RoundRectangle.TopRightCorner | D.RoundRectangle.BottomLeftCorner | D.RoundRectangle.BottomRightCorner
                    }
                }
            }
        }
    }
}
