// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import org.deepin.dtk 1.0 as D

Item {
    id: root
    property string userId
    property string filter: "icons/local"
    property string currentAvatar

    signal requireFileDialog()

    implicitHeight: 100
    width: parent ? parent.width : 460

    Component.onCompleted: {
        const cached = dccData.customAvatarFromCache()
        if (cached && cached.length > 0) {
            root.currentAvatar = cached
        } else {
            const icons = dccData.avatars(root.userId, root.filter, "")
            if (icons && icons.length > 1)
                root.currentAvatar = icons[1]
        }
        const tmp = row.icons
        row.icons = []
        Qt.callLater(function(){ row.icons = tmp })
    }

    RowLayout {
        id: row
        spacing: 12
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        property var allIcons: dccData.avatars(root.userId, root.filter, "")
        property var icons: allIcons.slice(0, Math.min(5, allIcons.length))

        Repeater {
            model: row.icons
            delegate: D.ItemDelegate {
                id: delegate
                property bool isAddButton: modelData == "add"
                property string modelPathNoQuery: (modelData || "")
                property string currentNoQuery: (root.currentAvatar || "")
                property bool isSelected: !isAddButton && (currentNoQuery === modelPathNoQuery)
                implicitHeight: 72
                implicitWidth: 72
                checkable: false
                hoverEnabled: true
                background: Item {}
                onClicked: {
                    if (!delegate.isAddButton)
                        root.currentAvatar = modelData
                }

                // add 按钮
                Loader {
                    active: delegate.isAddButton
                    sourceComponent: Item {
                        implicitHeight: 72
                        implicitWidth: 72
                        Button {
                            id: control
                            width: 60
                            height: 60
                            padding: 0
                            leftPadding: 0
                            rightPadding: 0
                            topPadding: 0
                            bottomPadding: 0
                            icon {
                                name: modelData
                                height: 60
                                width: 60
                            }
                            anchors.centerIn: parent
                            onClicked: root.requireFileDialog()
                        }
                    }
                }

                // 普通头像项
                Item {
                    id: iconSlot
                    width: 60
                    height: 60
                    anchors.centerIn: parent
                    clip: true

                    Rectangle {
                        id: avatarContainer
                        width: 60
                        height: 60
                        radius: 6
                        anchors.centerIn: parent
                        clip: true
                        visible: !delegate.isAddButton

                        Image {
                            id: img
                            source: modelData && !delegate.isAddButton ? modelData : ""
                            width: parent.width
                            height: parent.height
                            fillMode: Image.PreserveAspectCrop
                            anchors.centerIn: parent
                            antialiasing: true
                            smooth: true
                        }

                        D.DciIcon {
                            id: fallbackImg
                            palette: D.DTK.makeIconPalette(delegate.palette)
                            mode: delegate.D.ColorSelector.controlState
                            theme: delegate.D.ColorSelector.controlTheme
                            fallbackToQIcon: false
                            name: modelData
                            sourceSize: Qt.size(60, 60)
                            anchors.centerIn: parent
                            visible: img.status === Image.Error || img.status === Image.Null
                            antialiasing: true
                        }
                    }
                }

                D.BoxShadow {
                    id: boxShadow
                    hollow: true
                    anchors.fill: iconSlot
                    shadowBlur: 3
                    spread: 3
                    shadowColor: delegate.palette.window
                    cornerRadius: 6
                }

                Rectangle {
                    id: background
                    radius: 8
                    anchors.fill: iconSlot
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
                    visible: delegate.hovered && !delegate.isSelected && !delegate.isAddButton
                    focusPolicy: Qt.NoFocus
                    anchors.right: parent.right
                    anchors.top: parent.top
                    icon {
                        name: "entry_clear"
                        width: 20
                        height: 20
                    }
                    background: Rectangle { radius: 10 }
                    onClicked: {
                        if (typeof dccData.deleteUserIcon === 'function') {
                            dccData.deleteUserIcon(root.userId, modelData)
                        }
                        root.refresh()
                    }
                }
            }
        }
    }

    function refresh() {
        row.allIcons = dccData.avatars(root.userId, root.filter, "")
        row.icons = row.allIcons.slice(0, Math.min(5, row.allIcons.length))
        const curNoQuery = (root.currentAvatar || "")
        if (curNoQuery && row.icons.every(function(p){ return (p||"") !== curNoQuery })) {
            const idx = row.allIcons.findIndex(function(p){ return (p||"") === curNoQuery })
            if (idx >= 0) {
                const addBtn = row.icons.length > 0 ? row.icons[0] : "add"
                const selected = row.allIcons[idx]
                let others = row.icons.slice(1, 5)
                others = others.filter(function(p){ return (p||"") !== curNoQuery })
                row.icons = [addBtn].concat([selected]).concat(others).slice(0, Math.min(5, row.allIcons.length))
            }
        }
        if (root.currentAvatar && row.allIcons.indexOf(root.currentAvatar) < 0) {
            const curNoQuery = (root.currentAvatar || "")
            const existsNoQuery = row.allIcons.some(function(p){ return (p||"") === curNoQuery })
            if (!existsNoQuery) root.currentAvatar = ""
        }
        root.currentAvatar = dccData.customAvatarFromCache() || root.currentAvatar
        const tmp = row.icons
        row.icons = []
        Qt.callLater(function(){ row.icons = tmp })
    }

    Connections {
        target: dccData
        function onAvatarChanged(userId, avatar) {
            if (userId === root.userId)
                root.refresh()
        }
    }
}


