// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

Item {
    id: root
    implicitWidth: DS.Style.itemDelegate.width
    implicitHeight: DS.Style.itemDelegate.height
    property bool separatorVisible: false // 分隔线
    property bool shadowVisible: true // 阴影
    property alias control: root.parent
    property real radius: DS.Style.control.radius

    property real backgroundType: 0
    property D.Palette backgroundColor: D.Palette {
        normal: root.palette.base
        normalDark: root.palette.base
        hovered: Qt.color("#E5E5E5")
        hoveredDark: Qt.color("#323232")
    }
    property D.Palette onlyHoveredColor: D.Palette {
        normal: root.palette.base
        normalDark: root.palette.base
        hovered: Qt.rgba(0, 0, 0, 0.1)
        hoveredDark: Qt.rgba(1, 1, 1, 0.1)
    }
    // 阴影
    Loader {
        y: 2
        z: 0
        width: parent.width
        height: parent.height
        active: shadowVisible && (!control.checked) && (backgroundType & 0x01) && (control.corners & D.RoundRectangle.BottomCorner)
        sourceComponent: D.RoundRectangle {
            color: palette.midlight
            radius: root.radius
            corners: control.corners
        }
    }
    // 背景
    Loader {
        z: 1
        anchors.fill: parent
        active: (backgroundType & 0x01) || ((backgroundType & 0x02) && control.hovered)
        // active: backgroundType !== 0 // (backgroundType & 0xFF) && !(backgroundType & 0x08)
        anchors.topMargin: ((backgroundType & 0x02) && control.hovered) && !(control.corners & D.RoundRectangle.TopCorner) ? 1 : 0
        anchors.bottomMargin: ((backgroundType & 0x02) && control.hovered) && !(control.corners & D.RoundRectangle.BottomCorner) ? 1 : 0
        sourceComponent: D.RoundRectangle {
            // 高亮时，hovered状态HighlightPanel有处理,无阴影时，hovered状态使用半透明
            color: ((backgroundType & 0x08) || (backgroundType & 0x02) === 0) ? palette.base : ((backgroundType & 0x01) ? root.D.ColorSelector.backgroundColor : root.D.ColorSelector.onlyHoveredColor)
            radius: root.radius
            corners: control.corners
        }
    }
    // 高亮
    Loader {
        z: 2
        anchors.fill: parent
        anchors.topMargin: 1
        anchors.bottomMargin: 1
        active: control.checked && !control.cascadeSelected
        sourceComponent: D.HighlightPanel {}
    }
    // Warning
    Loader {
        z: 2
        anchors.fill: parent
        anchors.topMargin: 1
        anchors.bottomMargin: 1
        active: (backgroundType & 0x10)
        sourceComponent: Rectangle {
            radius: root.radius
            color: Qt.rgba(0.95, 0.22, 0.20, 0.15)
        }
    }
    // 分隔线
    Loader {
        active: separatorVisible && index !== 0
        height: 2
        z: 3
        anchors {
            bottom: parent.top
            bottomMargin: control.ListView.view ? (control.ListView.view.spacing - 2) / 2 : -1
            // bottomMargin: (control.ListView.view.spacing - 1) / 2
            left: parent.left
            leftMargin: 10
            right: parent.right
            rightMargin: 10
        }
        sourceComponent: Rectangle {
            color: palette.window
        }
    }
}
