// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

ListView {
    id: listview
    // 132 = 130 + itemBorderWidth
    readonly property int itemWidth: 132
    readonly property int itemHeight: 107
    readonly property int itemBorderWidth: 2
    readonly property int itemSpacing: 50
    readonly property int imageRectH: 78 // 84
    readonly property int imageRectW: itemWidth

    readonly property int gridMaxColumns: Math.floor(listview.width / (itemWidth + itemSpacing))
    readonly property int gridMaxRows: 2
    model: Math.ceil((dccData.globalThemeModel.rowCount()  + 1) / (2 * gridMaxColumns))
    spacing: 0
    clip: true
    orientation: ListView.Horizontal
    layoutDirection: Qt.LeftToRight
    snapMode: ListView.SnapOneItem
    boundsBehavior: Flickable.StopAtBounds
    highlightRangeMode: ListView.StrictlyEnforceRange
    highlightMoveDuration: 400
    delegate: Item {
        width: listview.width
        height: listview.height
        D.SortFilterModel {
            id: sortedModel
            model: dccData.globalThemeModel
            property int pageIndex: index
            property int maxCount: listview.gridMaxColumns * listview.gridMaxRows
            lessThan: function(left, right) {
                return left.index < right.index
            }
            filterAcceptsItem: function(item) {
                return item.index >= index * maxCount && item.index < (index + 1) * maxCount
            }
            onMaxCountChanged: {
                sortedModel.update()
            }

            delegate: Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "transparent"
                ToolTip.visible: mouseArea.containsMouse && model.toolTip !== ""
                ToolTip.text: model.toolTip
                ToolTip.delay: 300
                ColumnLayout {
                    width: listview.itemWidth
                    height: listview.itemHeight
                    anchors.centerIn: parent

                    Item {
                        Layout.preferredHeight: listview.imageRectH
                        Layout.preferredWidth: listview.imageRectW

                        Rectangle {
                            anchors.fill: parent
                            visible: model.checked
                            color: "transparent"
                            border.width: 2
                            border.color: D.DTK.platformTheme.activeColor
                            radius: 7
                        }

                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: listview.itemBorderWidth + 1
                            color: "transparent"
                            radius: 7

                            Image {
                                anchors.fill: parent
                                sourceSize: Qt.size(parent.width, parent.height)
                                source: model.pic
                                asynchronous: true
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    Text {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        text: model.id
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: model.checked ? D.DTK.platformTheme.activeColor : this.palette.windowText
                    }
                }
                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        dccData.worker.setGlobalTheme(model.id)
                    }
                }
            }
        }
        GridLayout {
            anchors.left: parent.left
            property int contentCount: repeater.count + 1
            width: contentCount < columns ? contentCount * (itemWidth + itemSpacing) : parent.width
            height: contentCount <= columns ? parent.height / 2 : parent.height
            rowSpacing: 0
            columnSpacing: 0
            rows: listview.gridMaxRows
            columns: listview.gridMaxColumns
            Repeater {
                id: repeater
                model: sortedModel
            }

            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                visible: index === listview.model - 1
                color: "transparent"
                ColumnLayout {
                    width: listview.itemWidth
                    height: listview.itemHeight
                    anchors.centerIn: parent

                    Item {
                        Layout.preferredHeight: listview.imageRectH
                        Layout.preferredWidth: listview.imageRectW

                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: listview.itemBorderWidth + 1
                            color: "transparent"
                            radius: 7

                            D.DciIcon {
                                anchors.fill: parent
                                sourceSize: Qt.size(parent.width, parent.height)
                                name: "download-more"
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    Text {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        text: "More Wallpapers"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: this.palette.windowText
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        dccData.worker.goDownloadTheme()
                    }
                }
            }
        }
    }
}
