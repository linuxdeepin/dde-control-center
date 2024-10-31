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
    readonly property int imageRectH: 120
    readonly property int imageRectW: 180
    readonly property int imageSpacing: 10
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
            font: D.DTK.fontManager.t4
            text: dccObj.displayName
        }
        Item {
            Layout.fillWidth: true
        }
        D.ToolButton {
            text: isExpand ? "收起" : `显示全部-${model.rowCount()}张`
            font: D.DTK.fontManager.t6
            flat: true
            onClicked: {
                isExpand = !isExpand
            }
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

        D.SortFilterModel {
            id: sortedModel
            model: root.model
            property int maxCount: Math.floor((layout.width + imageSpacing) / (imageRectW + imageSpacing)) * 2
            lessThan: function(left, right) {
                return left.index < right.index
            }
            filterAcceptsItem: function(item) {
                return isExpand ? true : item.index < maxCount
            }
            onMaxCountChanged: {
                sortedModel.update()
            }

            delegate: Control {
                id: control
                width: root.imageRectW
                height: root.imageRectH
                hoverEnabled: true

                contentItem: Item {
                    readonly property int imageMargin: 3

                    Image {
                        property bool isDarktype: true
                        anchors.centerIn : parent
                        id: img2x2
                        width: 2
                        height: 2
                        source: imageSource
                        fillMode: Image.Stretch
                        Component.onCompleted: {
                            img2x2.grabToImage(function(result) {
                                isDarktype = dccData.imageHelper.isDarkType(result.image);
                            });
                        }
                    }

                    Image {
                        anchors.fill: parent
                        anchors.margins: parent.imageMargin
                        id: image
                        source: model.url
                        sourceSize: Qt.size(image.width, image.height)
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
                        radius: 9
                    }

                    OpacityMask {
                        anchors.fill: parent
                        anchors.margins: parent.imageMargin
                        source: image
                        maskSource: Rectangle {
                            implicitWidth: image.width
                            implicitHeight: image.height
                            radius: 8
                        }
                    }
                    Control {
                        implicitHeight: 24
                        implicitWidth: 24
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.topMargin: - height / 2 + parent.imageMargin
                        anchors.rightMargin: - width / 2 + parent.imageMargin
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
                                root.wallpaperSelected(model.url, model.isDark, false)
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
                            text: "设置锁屏"
                            onTriggered: {
                                root.wallpaperSelected(model.url, img2x2.isDarktype, true)
                            }
                        }
                        MenuItem {
                            text: "全部设置"
                            onTriggered: {
                                root.wallpaperSelected(model.url, img2x2.isDarktype, true)
                                root.wallpaperSelected(model.url, img2x2.isDarktype, false)
                            }
                        }
                    }
                }
            }
        }
        Flow {
            id: layout
            width: parent.width
            spacing: imageSpacing
            bottomPadding: imageSpacing

            move: Transition {
            }

            Repeater {
                model: sortedModel
            }
        }
    }
}
