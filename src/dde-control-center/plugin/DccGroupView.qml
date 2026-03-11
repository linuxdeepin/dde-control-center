// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import QtQml.Models //Delegatechoice for Qt >= 6.9
import Qt.labs.qmlmodels //DelegateChooser

import org.deepin.dtk 1.0 as D
import org.deepin.dcc 1.0

Rectangle {
    id: control
    property bool isGroup: true
    property alias spacing: layoutView.spacing
    property var model: DccModel {
        root: dccObj
    }

    // 判断当前项是否是第一个可交互的菜单项（Menu或MenuEditor类型）
    function isFirstInteractiveItem(idx) {
        for (var i = 0; i < idx; i++) {
            var obj = control.model.getObject(i)
            if (obj && (obj.pageType === DccObject.Menu || obj.pageType === DccObject.MenuEditor)) {
                return false
            }
        }
        var currentObj = control.model.getObject(idx)
        return currentObj && (currentObj.pageType === DccObject.Menu || currentObj.pageType === DccObject.MenuEditor)
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
            model: control.model

            delegate: DelegateChooser {
                role: "pageType"
                DelegateChoice {
                    roleValue: DccObject.Menu
                    delegate: DccMenuItem {
                        topInset: control.isGroup ? 0 : 3
                        bottomInset: control.isGroup ? 0 : 3
                        separatorVisible: control.isGroup
                        backgroundType: model.item.backgroundType | DccObject.ClickStyle
                        Layout.fillWidth: true
                        activeFocusOnTab: control.isFirstInteractiveItem(index)
                        corners: control.isGroup ? getCornersForBackground(index, repeater.count) : D.RoundRectangle.TopLeftCorner | D.RoundRectangle.TopRightCorner | D.RoundRectangle.BottomLeftCorner | D.RoundRectangle.BottomRightCorner
                    }
                }
                DelegateChoice {
                    roleValue: DccObject.Editor
                    delegate: DccEditorItem {
                        topInset: control.isGroup ? 0 : 3
                        bottomInset: control.isGroup ? 0 : 3
                        separatorVisible: control.isGroup
                        backgroundType:model.item.backgroundType | (control.isGroup ? 1 : 0)
                        Layout.fillWidth: true
                        activeFocusOnTab: false
                        corners: control.isGroup ? getCornersForBackground(index, repeater.count) : D.RoundRectangle.TopLeftCorner | D.RoundRectangle.TopRightCorner | D.RoundRectangle.BottomLeftCorner | D.RoundRectangle.BottomRightCorner
                    }
                }
                DelegateChoice {
                    roleValue: DccObject.Item
                    delegate: DccItem {
                        topInset: control.isGroup ? 0 : 3
                        bottomInset: control.isGroup ? 0 : 3
                        separatorVisible: control.isGroup
                        backgroundType: model.item.backgroundType | (control.isGroup ? 1 : 0)
                        Layout.fillWidth: true
                        activeFocusOnTab: false
                        corners: control.isGroup ? getCornersForBackground(index, repeater.count) : D.RoundRectangle.TopLeftCorner | D.RoundRectangle.TopRightCorner | D.RoundRectangle.BottomLeftCorner | D.RoundRectangle.BottomRightCorner
                    }
                }
                DelegateChoice {
                    roleValue: DccObject.MenuEditor
                    delegate: DccMenuEditorItem {
                        topInset: control.isGroup ? 0 : 3
                        bottomInset: control.isGroup ? 0 : 3
                        separatorVisible: control.isGroup
                        backgroundType: model.item.backgroundType | DccObject.ClickStyle
                        Layout.fillWidth: true
                        activeFocusOnTab: control.isFirstInteractiveItem(index)
                        corners: control.isGroup ? getCornersForBackground(index, repeater.count) : D.RoundRectangle.TopLeftCorner | D.RoundRectangle.TopRightCorner | D.RoundRectangle.BottomLeftCorner | D.RoundRectangle.BottomRightCorner
                    }
                }
            }
        }
    }
}
