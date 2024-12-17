// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import org.deepin.dtk 1.0 as D
import QtQuick.Effects

GridView {
    id: gridView
    property string headerText
    property string currentAvatar
    property int columsCount: 5
    property bool isCustom: false
    cellHeight: 90
    cellWidth: 90
    implicitHeight: Math.ceil(count / 5) * cellHeight + (headerText.length > 0 ? 40 : 0)
    implicitWidth: cellWidth * columsCount
    clip: true
    keyNavigationEnabled: true
    interactive: false

    signal removeCustomAvatar(string filePath)
    signal requireFileDialog()

    header: Loader {
        active: headerText.length > 0
        sourceComponent: Rectangle {
            anchors.leftMargin: 10
            width: 120
            height: 40
            color: "transparent"

            Text {
                text: gridView.headerText
                font.pointSize: 16
                anchors.left: parent.left
                anchors.leftMargin: 10
                color: palette.text
            }
        }
    }

    delegate: D.ItemDelegate {
        id: delegate
        property bool isAddButton: modelData == "add"
        implicitHeight: 90
        implicitWidth: 90
        checkable: !isAddButton
        checked: !isAddButton && (gridView.currentAvatar === modelData)
        onCheckedChanged: {
            if (checked)
                gridView.currentAvatar = modelData
        }

        Loader {
            active: delegate.isAddButton
            sourceComponent: Item {
                implicitHeight: 90
                implicitWidth: 90
                Button {
                    id: control
                    icon {
                        name: modelData
                        // TODO: icon size not Work!! dtk bug https://github.com/linuxdeepin/dtk/issues/207
                        height: 64
                        width: 64
                    }
                    anchors {
                        fill: parent
                        margins: 5
                    }
                    onClicked: {
                        requireFileDialog()
                    }
                }

            }
        }

        D.DciIcon {
            id: img
            palette: D.DTK.makeIconPalette(delegate.palette)
            mode: delegate.D.ColorSelector.controlState
            theme: delegate.D.ColorSelector.controlTheme
            fallbackToQIcon: false
            name: modelData
            sourceSize: Qt.size(80, 80)
            anchors.centerIn: parent
            visible: !delegate.isAddButton // false for MultiEffect need
            antialiasing: true
        }

        // fake round image
        D.BoxShadow {
            id: boxShadow
            hollow: true
            anchors.fill: img
            shadowBlur: 3
            spread: 3
            shadowColor: delegate.palette.window
            cornerRadius: 6
        }

        // antialiasing not work well
        // MultiEffect {
        //     source: img
        //     anchors.centerIn: parent
        //     width: img.width
        //     height: img.height
        //     maskEnabled: true
        //     maskSource: mask
        //     maskThresholdMin: 0.5
        //     maskSpreadAtMin: 0.0
        //     antialiasing: true
        // }
        // Rectangle {
        //     id: mask
        //     antialiasing: true
        //     anchors.fill: img
        //     layer.enabled: true
        //     visible: false
        //     clip: true
        //     radius: 6
        // }

        // TODO: Not work well...
        // D.ItemViewport {
        //     id: viewPort
        //     sourceItem: img
        //     radius: 6
        //     fixed: true
        //     width: img.sourceSize.width
        //     height: img.sourceSize.height
        //     hideSource: true
        //     antialiasing: true
        //     anchors.centerIn: parent
        // }

        background: Rectangle {
            radius: 8
            anchors.centerIn: parent
            implicitHeight: 86
            implicitWidth: 86
            color: "transparent"
            border.color: delegate.palette.highlight
            border.width: 1
            visible: delegate.checked
            z: 10
        }

        D.IconLabel {
            id: checkIcon
            z: 99
            icon.name: "item_checked"
            icon.width: 20
            icon.height: 20
            anchors.right: parent.right
            anchors.top: parent.top
            visible: delegate.checked
        }

        D.ActionButton {
            z: 99
            visible: gridView.isCustom && !checkIcon.visible && !delegate.isAddButton
            focusPolicy: Qt.NoFocus
            anchors.right: parent.right
            anchors.top: parent.top
            icon {
                name: "entry_clear"
                width: 20
                height: 20
            }
            background: Rectangle {
                radius: 10
            }

            onClicked: {
                console.log("need remove custom avatar..", modelData)
                removeCustomAvatar(modelData)
            }
        }
    }
}
