// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt5Compat.GraphicalEffects

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dtk.private as P
import org.deepin.dcc.personalization 1.0

ColumnLayout {
    id: root
    spacing: 0
    readonly property int imageRectW: 84
    readonly property int imageRectH: 54
    readonly property int imageBorder: 2
    // 1 is the distance between the border and the image
    readonly property int imageMargin: imageBorder + 1
    readonly property int itemWidth: imageRectW + imageBorder * 2 + 1
    readonly property int itemHeight: imageRectH + imageBorder * 2 + 1
    readonly property int imageSpacing: 7

    property var model
    property bool isExpand: false
    property var currentItem
    property string firstItemImgSource: ""
    property bool firstItemVisible: firstItemImgSource !== ""
    property bool enableContextMenu: true

    signal wallpaperSelected(var url, bool isDark, var option)
    signal firstItemClicked()
    signal wallpaperDeleteClicked(var url)

    onIsExpandChanged: {
        sortedModel.update()
    }

    RowLayout {
        id: titleLayout
        Layout.fillWidth: true
        Layout.preferredHeight: Math.max(titleLabel.implicitHeight, titleLoolButton.implicitHeight)
        Layout.topMargin: 6
        Layout.bottomMargin: 0

        Label {
            id: titleLabel
            Layout.leftMargin: 10
            font: D.DTK.fontManager.t6
            text: dccObj.displayName
        }
        Item {
            Layout.fillWidth: true
        }
        D.ToolButton {
            id: titleLoolButton
            visible: layout.lineCount * 2 < root.model.count + root.firstItemVisible ? 1 : 0
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
            text: root.isExpand ? qsTr("unfold") : qsTr("show all - %1 items").arg(root.model.count)
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
            bottomPadding: 12
            anchors.horizontalCenter: parent.horizontalCenter

            move: Transition {
            }

            Loader {
                active: root.firstItemVisible
                sourceComponent: Item {
                    width: root.itemWidth
                    height: root.itemHeight

                    Control {
                        id: iconControl
                        anchors.fill: parent                        
                        contentItem: D.DciIcon {
                            id: firstItemImage
                            anchors.fill: parent
                            visible: false
                            sourceSize: Qt.size(width, height)
                            name: root.firstItemImgSource
                            asynchronous: true
                            theme: iconControl.D.ColorSelector.controlTheme
                        }
                    }

                    OpacityMask {
                        anchors.fill: parent
                        anchors.margins: root.imageMargin
                        source: firstItemImage
                        maskSource: Rectangle {
                            anchors.fill: parent
                            implicitWidth: firstItemImage.width
                            implicitHeight: firstItemImage.height
                            radius: 6
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            root.firstItemClicked()
                        }
                    }
                }
            }

            Repeater {
                model: sortedModel
            }
        }

        Timer {
            id: updateTimer
            interval: 20
            running: false
            repeat: false
            onTriggered: {
                sortedModel.update()
            }
        }

        D.SortFilterModel {
            id: sortedModel
            model: root.model
            property int maxCount: layout.lineCount * 2 - (root.firstItemVisible ? 1 : 0)
            property int lastMaxCount: 0
            lessThan: function(left, right) {
                return left.index < right.index
            }
            filterAcceptsItem: function(item) {
                return root.isExpand ? true : item.index < maxCount
            }
            onMaxCountChanged: {
                if ((Math.abs(maxCount - lastMaxCount)) > 5) {
                    updateTimer.start()
                } else {
                    sortedModel.update()
                }
                lastMaxCount = maxCount
            }

            delegate: Control {
                id: control
                width: root.itemWidth
                height: root.itemHeight
                hoverEnabled: true

                contentItem: Item {
                    id: wallpaperItem
                    function requestSetWallpaper(option) {
                        img2x2.grabToImage(function(result) {
                            const isDarkType = dccData.imageHelper.isDarkType(result.image)
                            root.wallpaperSelected(model.url, isDarkType, option)
                        }, Qt.size(2, 2))
                    }

                    Image {
                        id: img2x2
                        property bool isDarktype: true
                        anchors.centerIn : parent
                        width: 2
                        height: 2
                        source: "image://DccImage/" + model.thumbnail
                        fillMode: Image.Stretch
                        asynchronous: true
                    }

                    Image {
                        id: image
                        anchors.centerIn: parent
                        width: root.imageRectW
                        height: root.imageRectH
                        sourceSize: Qt.size(width, height)
                        source: "image://DccImage/" + model.thumbnail
                        mipmap: true
                        visible: false
                        fillMode: Image.PreserveAspectCrop
                        asynchronous: true
                        retainWhileLoading: true
                    }

                    Rectangle {
                        id: borderRect
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

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        onClicked: {
                            if (mouse.button === Qt.LeftButton) {
                                wallpaperItem.requestSetWallpaper(PersonalizationExport.Option_All)
                            } else if (mouse.button === Qt.RightButton && root.enableContextMenu && !DccApp.isTreeland()) {
                                contextMenu.x = mouse.x
                                contextMenu.y = mouse.y
                                contextMenu.open()
                            }
                        }
                    }

                    Control {
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.topMargin: - height / 2 + root.imageMargin
                        anchors.rightMargin: - width / 2 + root.imageMargin + 2
                        hoverEnabled: true
                        z: 999
                        contentItem: D.ActionButton {
                            icon.name: "close"
                            implicitWidth: 20
                            implicitHeight: 20
                            icon.width: 14
                            icon.height: 14
                            visible: (control.hovered || parent.hovered) && model.deleteAble && !model.selected
                            background: D.BoxPanel {
                                radius: width / 2
                                enableBoxShadow: true
                                boxShadowBlur: 10
                                boxShadowOffsetY: 4
                                color1: D.Palette {
                                    normal {
                                        common: Qt.rgba(240 / 255.0, 240 / 255.0, 240 / 255.0, 0.9)
                                        // TODO crystal: Qt.rgba(240 / 255.0, 240 / 255.0, 240 / 255.0, 0.5)
                                        crystal: Qt.rgba(240 / 255.0, 240 / 255.0, 240 / 255.0, 1.0)
                                    }
                                    normalDark {
                                        common: Qt.rgba(24 / 255.0, 24 / 255.0, 24 / 255.0, 0.8)
                                        // TODO crystal: Qt.rgba(240 / 255.0, 240 / 255.0, 240 / 255.0, 0.5)
                                        crystal: Qt.rgba(24 / 255.0, 24 / 255.0, 24 / 255.0, 1.0)
                                    }
                                    hovered {
                                        common: Qt.rgba(220 / 255.0, 220 / 255.0, 220 / 255.0, 0.95)
                                        crystal: Qt.rgba(220 / 255.0, 220 / 255.0, 220 / 255.0, 1.0)
                                    }
                                    hoveredDark {
                                        common: Qt.rgba(44 / 255.0, 44 / 255.0, 44 / 255.0, 0.9)
                                        crystal: Qt.rgba(44 / 255.0, 44 / 255.0, 44 / 255.0, 1.0)
                                    }
                                }

                                color2: color1
                                insideBorderColor: null
                                outsideBorderColor: null
                            }
                            onClicked: {
                                root.wallpaperDeleteClicked(model.url)
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

                    D.Menu {
                        id: contextMenu
                        MenuItem {
                            text: qsTr("Set lock screen")
                            onTriggered: {
                                wallpaperItem.requestSetWallpaper(PersonalizationExport.Option_Lock)
                            }
                        }
                        MenuItem {
                            text: qsTr("Set desktop")
                            onTriggered: {
                                wallpaperItem.requestSetWallpaper(PersonalizationExport.Option_Desktop)
                            }
                        }
                    }
                }
            }
        }
    }
}
