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
    Connections {
        target: DccApp
        function onActiveItemChanged(item) {
            root.contentY = root.contentY + item.mapToItem(root, 0, 0).y
        }
    }
}
