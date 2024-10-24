// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.2
import QtQuick.Layouts 1.15

Flickable {
    id: root
    property real spacing: 0
    property bool isGroup: false

    contentHeight: centralItem.height + bottomItem.height + 10
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
    Control {
        id: centralItem
        focusPolicy: Qt.TabFocus
        hoverEnabled: false
        focus: true
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: 60
            rightMargin: 60
        }
    }
    Rectangle {
        color: this.palette.window
        implicitHeight: bottomItem.implicitHeight + 10
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: 60
            rightMargin: 60
        }
        // 防止鼠标穿透
        MouseArea {
            anchors.fill: parent
        }
        Control {
            id: bottomItem
            focusPolicy: Qt.TabFocus
            focus: true
            anchors {
                fill: parent
                topMargin: 5
                bottomMargin: 5
                leftMargin: 10
                rightMargin: 10
            }
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
            if (!panel.item || !panel.item.visible || !root.visible || panel.cnt > 5) {
                panel.visible = false
                panel.cnt = 1
                panel.item = undefined
                stop()
            } else {
                let itemY = panel.item.mapToItem(root, 0, 0).y
                let rHeight = root.height - (bottomItem.height + 10)
                if ((itemY + panel.item.height) > rHeight) {
                    root.contentY = itemY + panel.item.height - rHeight + root.contentY
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
    Component.onCompleted: {
        if (dccObj.children.length === 2) {
            if (!dccObj.children[0].page) {
                dccObj.children[0].page = groupView
            }
            centralItem.contentItem = dccObj.children[0].getSectionItem(centralItem)
            if (!dccObj.children[1].page) {
                dccObj.children[1].page = footer
            }
            bottomItem.contentItem = dccObj.children[1].getSectionItem(bottomItem)
        } else {
            console.warn(dccObj.name, " SettingsView must contain two sub items", dccObj.children.length)
        }
    }
}
