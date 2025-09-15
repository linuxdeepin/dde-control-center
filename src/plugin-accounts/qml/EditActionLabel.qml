// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

D.LineEdit {
    id: edit
    property alias editBtn: editButton
    property alias alertText: panel.alertText
    property alias showAlert: panel.showAlert
    property alias metrics: fontMetrics
    property var completeText: ""
    signal finished()

    readOnly: true
    horizontalAlignment: TextInput.AlignLeft
    verticalAlignment: TextInput.AlignVCenter
    topPadding: 4
    bottomPadding: 4
    clearButton.visible: !readOnly
    rightPadding: clearButton.width + clearButton.anchors.rightMargin
    implicitHeight: 36
    background: D.EditPanel {
        id: panel
        control: edit
        showBorder: !readOnly
        alertDuration: 3000
        implicitWidth: DS.Style.edit.width
        implicitHeight: 32
        anchors {
            fill: parent
            topMargin: 3
            bottomMargin: 3
        }
        backgroundColor: D.Palette {
            normal: Qt.rgba(1, 1, 1, 0)
            normalDark: Qt.rgba(1, 1, 1, 0)
        }
    }
    onEditingFinished: {
        if (edit.readOnly)
            return
        if (showAlert)
            showAlert = false

        finished()
    }

    FontMetrics {
        id: fontMetrics
        font: edit.font
    }

    D.ActionButton {
        id: editButton
        focusPolicy: Qt.StrongFocus
        width: 30
        height: 30
        icon.name: "dcc-edit"
        icon.width: DS.Style.edit.actionIconSize
        icon.height: DS.Style.edit.actionIconSize
        hoverEnabled: true
        background: Rectangle {
            anchors.fill: parent
            property D.Palette pressedColor: D.Palette {
                normal: Qt.rgba(0, 0, 0, 0.2)
                normalDark: Qt.rgba(1, 1, 1, 0.25)
            }
            property D.Palette hoveredColor: D.Palette {
                normal: Qt.rgba(0, 0, 0, 0.1)
                normalDark: Qt.rgba(1, 1, 1, 0.1)
            }
            radius: DS.Style.control.radius
            color: parent.pressed ? D.ColorSelector.pressedColor : (parent.hovered ? D.ColorSelector.hoveredColor : "transparent")
            border {
                color: parent.palette.highlight
                width: parent.visualFocus ? DS.Style.control.focusBorderWidth : 0
            }
        }
        anchors {
            right: edit.right
            verticalCenter: edit.verticalCenter
        }
        onClicked: {
            edit.readOnly = false
            edit.selectAll()
            edit.forceActiveFocus()
        }
    }
}
