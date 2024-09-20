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
            isGroup: isGroup
            spacing: spacing
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
        color: root.palette.window
        implicitHeight: bottomItem.implicitHeight + 10
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: 60
            rightMargin: 60
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

    Connections {
        target: DccApp
        function onActiveItemChanged(item) {
            root.contentY = root.contentY + item.mapToItem(root, 0, 0).y
        }
    }
    Component.onCompleted: {
        if (dccObj.children.length === 2) {
            if (!dccObj.children[0].page) {
                dccObj.children[0].page = groupView
            }
            centralItem.contentItem = dccObj.children[0].getSectionItem()
            if (!dccObj.children[1].page) {
                dccObj.children[1].page = footer
            }
            bottomItem.contentItem = dccObj.children[1].getSectionItem()
        } else {
            console.warn(dccObj.name, " SettingsView must contain two sub items", dccObj.children.length)
        }
    }
}
