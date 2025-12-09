// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.2
import QtQuick.Layouts 1.15
import "DccUtils.js" as DccUtils

Flickable {
    id: control
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
            isGroup: control.isGroup
            spacing: control.spacing
        }
    }
    Component {
        id: footer
        DccRowView {}
    }
    Item {
        y: control.contentY
        height: control.height - bottomItem.height
        width: parent.width
        clip: true
        DccLoader {
            id: centralItem
            y: -control.contentY
            focus: true
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: control.margin
                rightMargin: control.margin
            }
        }
    }
    DccLoader {
        id: bottomItem
        height: item ? (item.implicitHeight + 10) : 0
        focus: true
        anchors {
            left: parent.left
            right: parent.right
            topMargin: 5
            bottomMargin: 5
            leftMargin: control.margin + 10
            rightMargin: control.margin + 10
        }
        y: (control.contentHeight - control.contentY > control.height ? control.height - this.height + control.contentY : control.contentHeight - this.height)
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
            if (!panel.item || !panel.item.visible || !control.visible || panel.cnt > 5) {
                panel.visible = false
                panel.cnt = 1
                panel.item = undefined
                stop()
            } else {
                let itemY = panel.item.mapToItem(control, 0, 0).y
                let rHeight = control.height - bottomItem.height
                if ((itemY + panel.item.height) > rHeight) {
                    control.contentY = itemY + panel.item.height - rHeight + control.contentY
                } else if (itemY < 0) {
                    control.contentY = panel.item.mapToItem(groupView, 0, 0).y
                }

                panel.x = panel.item.mapToItem(control, 0, 0).x
                panel.y = panel.item.mapToItem(control, 0, 0).y + control.contentY
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

            let itemY = focusItem.mapToItem(control, 0, 0).y
            let rHeight = control.height - bottomItem.height
            if ((itemY + focusItem.height) > rHeight) {
                control.contentY = itemY + focusItem.height - rHeight + control.contentY
            } else if (itemY < 0) {
                control.contentY = focusItem.mapToItem(centralItem.contentItem, 0, 0).y
            }
        }
    }
    function updateItem() {
        if (!initItem && dccObj.children.length === 2) {
            if (!dccObj.children[0].page) {
                dccObj.children[0].page = groupView
            }
            centralItem.dccObj = dccObj.children[0]
            if (!dccObj.children[1].page) {
                dccObj.children[1].page = footer
            }
            bottomItem.dccObj = dccObj.children[1]
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
