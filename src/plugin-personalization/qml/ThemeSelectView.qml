// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

FocusScope {
    id: root
    activeFocusOnTab: true

    property alias currentIndex: listview.currentIndex
    property alias count: listview.count

    function incrementCurrentIndex() {
        listview.incrementCurrentIndex()
    }

    function decrementCurrentIndex() {
        listview.decrementCurrentIndex()
    }

    property int focusedThemeIndex: 0
    // 记录上次的模型数量，用于判断是否真正需要重置焦点
    property int lastModelCount: dccData.globalThemeModel ? dccData.globalThemeModel.rowCount() : 0
    property bool hadLostFocus: false

    onActiveFocusChanged: {
        if (activeFocus) {
            if (hadLostFocus) {
                focusedThemeIndex = 0
                hadLostFocus = false
            }
        } else {
            hadLostFocus = true
        }
    }

    Keys.onLeftPressed: {
        if (focusedThemeIndex > 0) {
            focusedThemeIndex--
            var pageIndex = Math.floor(focusedThemeIndex / (listview.gridMaxColumns * listview.gridMaxRows))
            if (pageIndex !== listview.currentIndex) {
                listview.currentIndex = pageIndex
            }
        }
    }

    Keys.onRightPressed: {
        var totalCount = dccData.globalThemeModel.rowCount() + 1 // +1 for "More Wallpapers"
        if (focusedThemeIndex < totalCount - 1) {
            focusedThemeIndex++
            var pageIndex = Math.floor(focusedThemeIndex / (listview.gridMaxColumns * listview.gridMaxRows))
            if (pageIndex !== listview.currentIndex) {
                listview.currentIndex = pageIndex
            }
        }
    }

    Keys.onUpPressed: {
        if (focusedThemeIndex >= listview.gridMaxColumns) {
            focusedThemeIndex -= listview.gridMaxColumns
        }
    }

    Keys.onDownPressed: {
        var totalCount = dccData.globalThemeModel.rowCount() + 1
        if (focusedThemeIndex + listview.gridMaxColumns < totalCount) {
            focusedThemeIndex += listview.gridMaxColumns
        }
    }

    Keys.onReturnPressed: activateCurrentItem()
    Keys.onEnterPressed: activateCurrentItem()
    Keys.onSpacePressed: activateCurrentItem()

    // IdRole value from ThemeVieweModel::UserDataRole (Qt::UserRole + 0x101 = 0x201)
    readonly property int themeIdRole: 0x201

    function activateCurrentItem() {
        var totalThemes = dccData.globalThemeModel.rowCount()
        if (focusedThemeIndex < totalThemes) {
            var themeId = dccData.globalThemeModel.data(dccData.globalThemeModel.index(focusedThemeIndex, 0), themeIdRole)
            listview.selectTheme = themeId
            dccData.worker.setGlobalTheme(themeId)
        } else {
            // "More Wallpapers" button
            dccData.worker.goDownloadTheme()
        }
    }

    ListView {
        id: listview
        anchors.fill: parent
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
                var newCount = dccData.globalThemeModel.rowCount()
                // 只有当模型数量真正发生变化时才重置焦点
                if (newCount !== root.lastModelCount) {
                    listview.selectTheme = ""
                    root.focusedThemeIndex = 0
                    root.lastModelCount = newCount
                }
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
                    property bool isFocused: root.activeFocus && root.focusedThemeIndex === model.index

                    ColumnLayout {
                        width: listview.itemWidth
                        height: listview.itemHeight
                        anchors.centerIn: parent

                        Rectangle {
                            Layout.preferredHeight: listview.imageRectH
                            Layout.preferredWidth: listview.imageRectW
                            border.width: 2
                            border.color: delegateRoot.isCurrent || delegateRoot.isFocused ? D.DTK.platformTheme.activeColor : "transparent"
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
                            root.focusedThemeIndex = model.index
                            listview.selectTheme = model.id
                            dccData.worker.setGlobalTheme(model.id)
                        }
                    }
                }
            }
            GridLayout {
                id: gridLayout
                anchors.left: parent.left
                property int contentCount: repeater.count + 1
                width: contentCount < columns ? contentCount * (listview.itemWidth + listview.itemSpacing) : parent.width
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
                    id: moreWallpapersItem
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    visible: index === listview.model - 1
                    color: "transparent"
                    property int itemGlobalIndex: dccData.globalThemeModel.rowCount()
                    property bool isFocused: root.activeFocus && root.focusedThemeIndex === itemGlobalIndex

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
                                border.width: moreWallpapersItem.isFocused ? 2 : 0
                                border.color: moreWallpapersItem.isFocused ? D.DTK.platformTheme.activeColor : "transparent"

                                Image {
                                    anchors.fill: parent
                                    anchors.margins: moreWallpapersItem.isFocused ? 2 : 0
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
}
