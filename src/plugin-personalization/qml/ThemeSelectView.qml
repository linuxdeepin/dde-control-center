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
    readonly property int itemHeight: 100
    readonly property int itemBorderWidth: 2
    readonly property int itemSpacing: 18
    readonly property int imageRectH: 86
    readonly property int imageRectW: itemWidth

    property int gridMaxColumns: Math.floor(listview.width / (itemWidth + itemSpacing))
    property int gridMaxRows: 2

    property string selectTheme: ""
    model: Math.ceil((dccData.globalThemeModel.rowCount()  + 1) / (2 * gridMaxColumns))
    spacing: 0
    clip: true
    orientation: ListView.Horizontal
    layoutDirection: Qt.LeftToRight
    snapMode: ListView.SnapOneItem
    boundsBehavior: Flickable.StopAtBounds
    highlightRangeMode: ListView.StrictlyEnforceRange
    highlightMoveDuration: 400

    Connections {
        target: dccData.globalThemeModel
        function onModelReset() {
            listview.selectTheme = ""
        }
    }

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
                id: delegateRoot
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "transparent"
                ToolTip.visible: mouseArea.containsMouse && model.toolTip !== ""
                ToolTip.text: model.toolTip
                ToolTip.delay: 300
                property bool isCurrent: listview.selectTheme === "" ? model.checked : listview.selectTheme === model.id
                ColumnLayout {
                    width: listview.itemWidth
                    height: listview.itemHeight
                    anchors.centerIn: parent
    
                    Rectangle {
                        Layout.preferredHeight: listview.imageRectH
                        Layout.preferredWidth: listview.imageRectW
                        border.width: 2
                        border.color: delegateRoot.isCurrent ? D.DTK.platformTheme.activeColor : "transparent"
                        color: "transparent"
                        radius: 8

                        Image {
                            anchors.fill: parent
                            anchors.margins: 4
                            mipmap: true
                            source: model.pic
                            sourceSize: Qt.size(width, height)
                            asynchronous: true
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    Text {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        text: model.name
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font: D.DTK.fontManager.t9
                        color: delegateRoot.isCurrent ? D.DTK.platformTheme.activeColor : this.palette.windowText
                    }
                }
                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        listview.selectTheme = model.id
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
                            radius: 8

                            Image {
                                anchors.fill: parent
                                mipmap: true
                                source: D.DTK.themeType === D.ApplicationHelper.LightType ? 
                                    "qrc:/icons/download_more_light.png" : "qrc:/icons/download_more_dark.png"
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
                        text: qsTr("More Wallpapers")
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: this.palette.windowText
                        font: D.DTK.fontManager.t9
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
