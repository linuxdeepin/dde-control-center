// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.2
import QtQuick.Layouts 1.15
import "DccUtils.js" as DccUtils

Flickable {
    id: root
    property real spacing: 0
    property bool isGroup: false
    property real margin: DccUtils.getMargin(width)
    property bool initItem: false

    contentHeight: centralItem.height + bottomItem.height
    ScrollBar.vertical: ScrollBar {
        width: 10
    }
    Component {
        id: groupView
        DccGroupView {
            isGroup: root.isGroup
            spacing: root.spacing
        }
    }
    Component {
        id: footer
        DccRowView {}
    }
    Item {
        y: root.contentY
        height: root.height - bottomItem.height
        width: parent.width
        clip: true
        Control {
            id: centralItem
            y: -root.contentY
            hoverEnabled: false
            focus: true
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: root.margin
                rightMargin: root.margin
            }
        }
    }
    Control {
        id: bottomItem
        implicitHeight: contentItem.implicitHeight + 10
        focus: true
        anchors {
            left: parent.left
            right: parent.right
            topMargin: 5
            bottomMargin: 5
            leftMargin: root.margin + 10
            rightMargin: root.margin + 10
        }
        y: (root.contentHeight - root.contentY > root.height ? root.height - this.implicitHeight + root.contentY : root.contentHeight - this.implicitHeight)
    }
    Rectangle {
        id: panel
        property var item: undefined
        property int cnt: 1
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
            if (!panel.item || !panel.item.visible || !root.visible || panel.cnt > 5) {
                panel.visible = false
                panel.cnt = 1
                panel.item = undefined
                stop()
            } else {
                let itemY = panel.item.mapToItem(root, 0, 0).y
                let rHeight = root.height - bottomItem.height
                if ((itemY + panel.item.height) > rHeight) {
                    root.contentY = itemY + panel.item.height - rHeight + root.contentY
                } else if (itemY < 0) {
                    root.contentY = panel.item.mapToItem(groupView, 0, 0).y
                }

                panel.x = panel.item.mapToItem(root, 0, 0).x
                panel.y = panel.item.mapToItem(root, 0, 0).y + root.contentY
                panel.height = panel.item.height
                panel.width = panel.item.width
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
    Connections {
        target: DccApp.mainWindow()
        function onActiveFocusItemChanged() {
            var focusItem = target.activeFocusItem
            var parentItem = focusItem
            while (parentItem && parentItem !== centralItem.contentItem) {
                parentItem = parentItem.parent
            }
            if (!parentItem || parentItem !== centralItem.contentItem) {
                return
            }

            let itemY = focusItem.mapToItem(root, 0, 0).y
            let rHeight = root.height - bottomItem.height
            if ((itemY + focusItem.height) > rHeight) {
                root.contentY = itemY + focusItem.height - rHeight + root.contentY
            } else if (itemY < 0) {
                root.contentY = focusItem.mapToItem(centralItem.contentItem, 0, 0).y
            }
        }
    }
    function updateItem() {
        if (!initItem && dccObj.children.length === 2) {
            if (!dccObj.children[0].page) {
                dccObj.children[0].page = groupView
            }
            centralItem.contentItem = dccObj.children[0].getSectionItem(centralItem)
            if (!dccObj.children[1].page) {
                dccObj.children[1].page = footer
            }
            bottomItem.contentItem = dccObj.children[1].getSectionItem(bottomItem)
            initItem = true
        }
    }
    Component.onCompleted: {
        updateItem()
    }
    Connections {
        target: dccObj
        function onChildrenChanged() {
            updateItem()
        }
    }
}
