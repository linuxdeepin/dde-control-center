// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import QtQuick.Effects

GridView {
    id: gridView
    property string headerText
    property string currentAvatar
    property int headerHeight: 40
    property int columsCount: 5
    property bool isCustom: false
    property int itemSize: 60
    property int spacing: 12
    cellHeight: itemSize + spacing
    cellWidth: itemSize + spacing
    implicitHeight: Math.ceil(count / columsCount) * cellHeight + (headerText.length > 0 ? headerHeight : 0)
    implicitWidth: cellWidth * columsCount
    clip: true
    keyNavigationEnabled: true
    interactive: false
    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
    Layout.leftMargin: 50
    Layout.rightMargin: 60
    Layout.bottomMargin: 10

    signal removeCustomAvatar(string filePath)
    signal requireFileDialog()

    header: Loader {
        active: headerText.length > 0
        sourceComponent: Rectangle {
            anchors.leftMargin: 10
            width: gridView.width
            height: headerLabel.implicitHeight + 2
            color: "transparent"
            Component.onCompleted: gridView.headerHeight = headerLabel.implicitHeight + 10

            Text {
                id: headerLabel
                text: gridView.headerText
                font: D.DTK.fontManager.t8
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.topMargin: 0
                anchors.leftMargin: 4
                color: palette.text
            }
        }
    }

    delegate: D.ItemDelegate {
        id: delegate
        property bool isAddButton: modelData == "add"
        property bool isSelected: !isAddButton && (gridView.currentAvatar === modelData)
        implicitHeight: gridView.cellHeight
        implicitWidth: gridView.cellWidth
        checkable: false
        onClicked: {
            if (!delegate.isAddButton)
                gridView.currentAvatar = modelData
        }

        Loader {
            active: delegate.isAddButton
            sourceComponent: Item {
                implicitHeight: gridView.cellHeight
                implicitWidth: gridView.cellWidth
                Button {
                    id: control
                    icon {
                        name: modelData
                        // TODO: icon size not Work!! dtk bug https://github.com/linuxdeepin/dtk/issues/207
                        height: gridView.itemSize
                        width: gridView.itemSize
                    }
                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
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
            sourceSize: Qt.size(gridView.itemSize, gridView.itemSize)
            anchors.left: parent.left
            anchors.leftMargin: 2
            anchors.verticalCenter: parent.verticalCenter
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
            id: background
            radius: 8
            anchors.fill: img
            anchors.margins: -2
            color: "transparent"
            border.color: delegate.palette.highlight
            border.width: 1
            visible: delegate.isSelected
            z: 10
        }

        D.IconLabel {
            id: checkIcon
            z: 99
            icon {
                name: "item_checked"
                width: 20
                height: 20
                palette: D.DTK.makeIconPalette(delegate.palette)
                mode: delegate.D.ColorSelector.controlState
                theme: delegate.D.ColorSelector.controlTheme
            }
            width: 20
            height: 20
            x: background.x + background.width - width / 2 - 3
            y: background.y - height / 2 + 5
            visible: delegate.isSelected
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
