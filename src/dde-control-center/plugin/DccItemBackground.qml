// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

Item {
    id: bgItem
    implicitWidth: DS.Style.itemDelegate.width
    implicitHeight: DS.Style.itemDelegate.height
    property bool separatorVisible: false // 分隔线
    property bool shadowVisible: true // 阴影
    property alias control: bgItem.parent
    property real radius: DS.Style.control.radius
    property real bgMargins: 3

    property real backgroundType: 0
    property D.Palette backgroundColor: D.Palette {
        normal: Qt.rgba(1, 1, 1, 1)
        normalDark: Qt.rgba(1, 1, 1, 0.05)
        hovered: Qt.rgba(0, 0, 0, 0.1)
        hoveredDark: Qt.rgba(1, 1, 1, 0.1)
        pressed: Qt.rgba(0, 0, 0, 0.15)
        pressedDark: Qt.rgba(1, 1, 1, 0.15)
    }
    property D.Palette bgColor: D.Palette {
        normal: backgroundColor.normal
        normalDark: backgroundColor.normalDark
    }
    property D.Palette shadowColor: D.Palette {
        normal: Qt.rgba(0, 0, 0, 0.05)
        normalDark: Qt.rgba(0, 0, 0, 0.3)
    }
    property D.Palette separatorColor: D.Palette {
        normal: Qt.rgba(0, 0, 0, 0.05)
        normalDark: Qt.rgba(1, 1, 1, 0.05)
    }

    D.ColorSelector.pressed: control.pressed && backgroundType & DccObject.Clickable

    // 阴影
    Loader {
        id: shadow
        y: 0
        z: 3
        width: parent.width
        height: parent.height
        active: shadowVisible && (!control.checked) && (backgroundType & 0x01) && (control.corners & D.RoundRectangle.BottomCorner)
        sourceComponent: Canvas {
            id: canvas
            property var color: bgItem.D.ColorSelector.shadowColor
            property real h: 1
            anchors.fill: parent
            renderTarget: Canvas.FramebufferObject
            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, canvas.width, canvas.height)
                ctx.fillStyle = canvas.color
                ctx.beginPath()
                ctx.moveTo(0, canvas.height / 2)
                ctx.lineTo(0, canvas.height - bgItem.radius)
                ctx.quadraticCurveTo(0, canvas.height, bgItem.radius, canvas.height)
                ctx.lineTo(canvas.width - bgItem.radius, canvas.height)
                ctx.quadraticCurveTo(canvas.width, canvas.height, canvas.width, canvas.height - bgItem.radius)
                ctx.lineTo(canvas.width, canvas.height / 2)
                ctx.closePath()
                ctx.fill()
                ctx.globalCompositeOperation = "destination-out"
                ctx.fillStyle = "black"
                ctx.beginPath()
                ctx.moveTo(0, canvas.height / 2 - canvas.h)
                ctx.lineTo(0, canvas.height - bgItem.radius - canvas.h)
                ctx.quadraticCurveTo(0, canvas.height - canvas.h, bgItem.radius, canvas.height - canvas.h)
                ctx.lineTo(canvas.width - bgItem.radius, canvas.height - canvas.h)
                ctx.quadraticCurveTo(canvas.width, canvas.height - canvas.h, canvas.width, canvas.height - bgItem.radius - canvas.h)
                ctx.lineTo(canvas.width, canvas.height / 2 - canvas.h)
                ctx.closePath()
                ctx.fill()
                ctx.globalCompositeOperation = "source-over"
            }
            Component.onCompleted: {
                canvas.requestPaint()
            }
            onColorChanged: {
                canvas.requestPaint()
            }
        }
    }
    // 背景
    Loader {
        z: 1
        anchors.fill: parent
        active: (backgroundType & 0x01) || ((backgroundType & 0x02) && control.hovered)
        // active: backgroundType !== 0 // (backgroundType & 0xFF) && !(backgroundType & 0x08)
        anchors.topMargin: 0
        anchors.bottomMargin: 0
        sourceComponent: D.RoundRectangle {
            // 高亮时，hovered状态HighlightPanel有处理,无阴影时，hovered状态使用半透明
            color: ((backgroundType & 0x08) || (backgroundType & 0x02) === 0 || !control.hoverEnabled) ? bgItem.D.ColorSelector.bgColor : bgItem.D.ColorSelector.backgroundColor
            radius: bgItem.radius
            corners: control.corners
        }
    }
    Loader {
        z: 2
        anchors {
            fill: parent
            topMargin: bgMargins
            bottomMargin: bgMargins
            leftMargin: bgMargins
            rightMargin: bgMargins
        }
        active: control.activeFocus || control.visualFocus
        sourceComponent: D.FocusBoxBorder {
            radius: bgItem.radius
            color: control.palette.highlight
        }
    }
    // 高亮
    Loader {
        z: 2
        anchors {
            fill: parent
            topMargin: bgMargins
            bottomMargin: bgMargins
            leftMargin: bgMargins
            rightMargin: bgMargins
        }
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
            radius: bgItem.radius
            color: Qt.rgba(0.95, 0.22, 0.20, 0.15)
        }
    }
    // 分隔线
    Loader {
        active: separatorVisible && (!(control.corners & D.RoundRectangle.BottomCorner))
        height: 1
        z: 3
        anchors {
            bottom: parent.bottom
            bottomMargin: 0
            // bottomMargin: (control.ListView.view.spacing - 1) / 2
            left: parent.left
            leftMargin: 10
            right: parent.right
            rightMargin: 10
        }
        sourceComponent: Rectangle {
            color: bgItem.D.ColorSelector.separatorColor
        }
    }
}
