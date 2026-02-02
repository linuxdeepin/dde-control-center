// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.2
import QtQuick.Layouts 1.15
import "DccUtils.js" as DccUtils

Flickable {
    id: control
    property alias spacing: groupView.spacing
    property alias isGroup: groupView.isGroup
    property real margin: DccUtils.getMargin(width)
    property bool scrollBarVisible: false

    contentHeight: groupView.height
    ScrollBar.vertical: ScrollBar {
        id: vbar
        width: 10
        opacity: control.scrollBarVisible ? 1 : 0
        Behavior on opacity {
            NumberAnimation { duration: 150 }
        }
    }
    DccGroupView {
        id: groupView
        isGroup: false
        height: implicitHeight + 10
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: control.margin
            rightMargin: control.margin
        }
    }
    Timer {
        id: showScrollBarTimer
        interval: 300
        repeat: false
        onTriggered: control.scrollBarVisible = true
    }
    Component.onCompleted: {
        showScrollBarTimer.start()
    }
    Rectangle {
        id: panel
        property var item: undefined
        property int cnt: 0
        property bool isIndicatorShown: true
        z: 10
        radius: 8
        color: "transparent"
        visible: false
        border.color: this.palette.highlight
        border.width: 2
    }
    Timer {
        interval: 100
        repeat: true
        running: panel.item !== undefined
        onTriggered: {
            if (!panel.item || !panel.item.visible || !control.visible || panel.cnt > 6) {
                panel.visible = false
                panel.cnt = 0
                panel.item = undefined
                panel.isIndicatorShown = false
                stop()
            } else {
                let itemY = panel.item.mapToItem(control, 0, 0).y
                if ((itemY + panel.item.height) > control.height) {
                    control.contentY = itemY + panel.item.height - control.height + control.contentY
                }
                itemY = panel.item.mapToItem(control, 0, 0).y
                if (itemY < 0) {
                    control.contentY = -itemY
                }
                if (panel.isIndicatorShown) {
                    panel.x = panel.item.mapToItem(control, 0, 0).x
                    panel.y = panel.item.mapToItem(control, 0, 0).y + control.contentY
                    panel.height = panel.item.height
                    panel.width = panel.item.width
                    panel.visible = panel.cnt & 1
                }
                panel.cnt++
            }
        }
    }
    Connections {
        target: DccApp
        function onActiveItemChanged(item, isIndicatorShown) {
            panel.item = item
            panel.isIndicatorShown = isIndicatorShown
        }
    }
    Connections {
        target: DccApp.mainWindow()
        function onActiveFocusItemChanged() {
            var focusItem = target.activeFocusItem
            var parentItem = focusItem
            while (parentItem && parentItem !== groupView) {
                parentItem = parentItem.parent
            }
            if (!parentItem || parentItem !== groupView) {
                return
            }

            let itemY = focusItem.mapToItem(control, 0, 0).y
            if ((itemY + focusItem.height) > control.height) {
                control.contentY = itemY + focusItem.height - control.height + control.contentY
            } else if (itemY < 0) {
                control.contentY = focusItem.mapToItem(groupView, 0, 0).y
            }
        }
    }
}
