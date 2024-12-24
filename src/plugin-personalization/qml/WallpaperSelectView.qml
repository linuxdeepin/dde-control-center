// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15
import QtQuick.Dialogs
import Qt5Compat.GraphicalEffects

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dtk.private as P

ColumnLayout {
    id: root
    property var model
    readonly property int imageRectW: 84
    readonly property int imageRectH: 54
    readonly property int imageBorder: 2
    // 1 is the distance between the border and the image
    readonly property int imageMargin: imageBorder + 1
    readonly property int itemWidth: imageRectW + imageBorder * 2 + 1
    readonly property int itemHeight: imageRectH + imageBorder * 2 + 1
    readonly property int imageSpacing: 5
    property bool isExpand: false
    property var currentItem

    signal wallpaperSelected(var url, bool isDark, bool isLock)

    onIsExpandChanged: {
        sortedModel.update()
    }

    RowLayout {
        id: titleLayout
        Layout.preferredHeight: 50
        Layout.fillWidth: true
        Label {
            Layout.leftMargin: 10
            font: D.DTK.fontManager.t6
            text: dccObj.displayName
        }
        Item {
            Layout.fillWidth: true
        }
        D.ToolButton {
            textColor: D.Palette {
                normal {
                    common: D.DTK.makeColor(D.Color.Highlight)
                }
                normalDark: normal
                hovered {
                    common: D.DTK.makeColor(D.Color.Highlight).lightness(+30)
                }
                hoveredDark: hovered
            }
            text: root.isExpand ? qsTr("unfold") : qsTr("show all") + `-${root.model.rowCount()}` + qsTr("items")
            onClicked: {
                root.isExpand = !root.isExpand
            }
            background: Item {}
        }
    }

    Item {
        Layout.fillWidth: true
        Layout.preferredHeight: containter.height

        // animation used
        Rectangle {
            id: containter
            width: parent.width
            height: layout.height
            color: "transparent"
            Behavior on height {
                NumberAnimation {
                    duration: 300
                    easing.type: Easing.OutQuart
                }
            }
        }

        Flow {
            id: layout
            property int lineCount: Math.floor((parent.width + root.imageSpacing) / (root.itemWidth + root.imageSpacing))
            width: lineCount * (root.itemWidth + root.imageSpacing) - root.imageSpacing
            spacing: root.imageSpacing
            bottomPadding: root.imageSpacing
            anchors.horizontalCenter: parent.horizontalCenter

            move: Transition {
            }

            Repeater {
                model: sortedModel
            }
        }

        D.SortFilterModel {
            id: sortedModel
            model: root.model
            property int maxCount: layout.lineCount * 2
            lessThan: function(left, right) {
                return left.index < right.index
            }
            filterAcceptsItem: function(item) {
                return root.isExpand ? true : item.index < maxCount
            }
            onMaxCountChanged: {
                sortedModel.update()
            }

            delegate: Control {
                id: control
                width: root.itemWidth
                height: root.itemHeight
                hoverEnabled: true

                contentItem: Item {
                    id: wallpaperItem
                    function requestSetWallpaper(isLock) {
                        img2x2.grabToImage(function(result) {
                            const isDarkType = dccData.imageHelper.isDarkType(result.image)
                            root.wallpaperSelected(model.url, isDarkType, isLock)
                        }, Qt.size(2, 2))
                    }

                    Image {
                        id: img2x2
                        property bool isDarktype: true
                        anchors.centerIn : parent
                        width: 2
                        height: 2
                        source: model.url
                        fillMode: Image.Stretch
                        asynchronous: true
                    }

                    Image {
                        id: image
                        anchors.centerIn: parent
                        width: root.imageRectW
                        height: root.imageRectH
                        sourceSize: Qt.size(width, height)
                        source: model.url
                        mipmap: true
                        visible: false
                        fillMode: Image.PreserveAspectCrop
                        asynchronous: true
                    }

                    Rectangle {
                        anchors.fill: parent
                        visible: model.url === root.currentItem && model.url !== undefined
                        color: "transparent"
                        border.width: 2
                        border.color: D.DTK.platformTheme.activeColor
                        radius: 8
                    }

                    OpacityMask {
                        anchors.fill: parent
                        anchors.margins: root.imageMargin
                        source: image
                        maskSource: Rectangle {
                            anchors.fill: parent
                            implicitWidth: image.width
                            implicitHeight: image.height
                            radius: 6
                        }
                    }
                    Control {
                        implicitHeight: 24
                        implicitWidth: 24
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.topMargin: - height / 2 + root.imageMargin
                        anchors.rightMargin: - width / 2 + root.imageMargin
                        hoverEnabled: true
                        contentItem: D.IconButton {
                            icon.name: "close"
                            // visible: control.hovered || parent.hovered
                            // FIXME: force false
                            visible: false
                            background: P.ButtonPanel {
                                implicitWidth: DS.Style.iconButton.backgroundSize
                                implicitHeight: DS.Style.iconButton.backgroundSize
                                radius: width / 2
                                button: control
                            }
                            scale: visible ? 1 : 0
                            Behavior on scale {
                                NumberAnimation {
                                    duration: 300
                                    easing.type: Easing.OutExpo
                                }
                            }
                        }
                    }


                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        onClicked: {
                            if (mouse.button === Qt.LeftButton) {
                                wallpaperItem.requestSetWallpaper(false)
                                wallpaperItem.requestSetWallpaper(true)
                            } else if (mouse.button === Qt.RightButton) {
                                contextMenu.x = mouse.x
                                contextMenu.y = mouse.y
                                contextMenu.open()
                            }
                        }
                    }

                    D.Menu {
                        id: contextMenu
                        MenuItem {
                            text: qsTr("Set lock screen")
                            onTriggered: {
                                wallpaperItem.requestSetWallpaper(true)
                            }
                        }
                        MenuItem {
                            text: qsTr("Set desktop")
                            onTriggered: {
                                wallpaperItem.requestSetWallpaper(false)
                            }
                        }
                    }
                }
            }
        }
    }
}
