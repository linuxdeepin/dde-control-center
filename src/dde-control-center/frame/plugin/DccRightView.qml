// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.2
import QtQuick.Layouts 1.15

Flickable {
    id: root
    property alias spacing: groupView.spacing
    property alias isGroup: groupView.isGroup

    contentHeight: groupView.height
    ScrollBar.vertical: ScrollBar {
        width: 10
    }
    DccGroupView {
        id: groupView
        isGroup: false
        height: implicitHeight + 10
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: 60
            rightMargin: 60
        }
    }
    Rectangle {
        id: panel
        property var item: undefined
        property int cnt: 0
        z: 10
        radius: 8
        color: "transparent"
        visible: false
        border.color: this.palette.highlight
        border.width: 2
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: 60
            rightMargin: 60
        }
    }
    Timer {
        interval: 100
        repeat: true
        running: panel.item !== undefined
        onTriggered: {
            if (!panel.item || !panel.item.visible || !root.visible || panel.cnt > 6) {
                panel.visible = false
                panel.cnt = 0
                panel.item = undefined
                stop()
            } else {
                let itemY = panel.item.mapToItem(root, 0, 0).y
                if ((itemY + panel.item.height) > root.height) {
                    root.contentY = itemY + panel.item.height - root.height + root.contentY
                }
                itemY = panel.item.mapToItem(root, 0, 0).y
                if (itemY < 0) {
                    root.contentY = -itemY
                }

                panel.y = panel.item.mapToItem(root, 0, 0).y + root.contentY
                panel.height = panel.item.height
                panel.visible = panel.cnt & 1
                panel.cnt++
            }
        }
    }
    Connections {
        target: DccApp
        function onActiveItemChanged(item) {
            panel.item = item
        }
    }
}
