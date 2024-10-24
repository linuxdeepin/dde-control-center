// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15

Label {
    id: root
    property alias hovered: mouseArea.containsMouse

    elide: Text.ElideRight
    ToolTip {
        visible: root.width < root.implicitWidth && root.hovered
        text: root.text
    }
    // 使用Attached方式退出时会崩溃
    // ToolTip.visible: width < implicitWidth && hovered
    // ToolTip.text: text
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.NoButton
    }
}
