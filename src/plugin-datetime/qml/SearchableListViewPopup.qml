// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window
import org.deepin.dtk 1.0
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dtk.private 1.0 as P

Loader {
    id: loader
    active: false
    property int maxVisibleItems: 10
    property int highlightedIndex: 0
    property string searchText: ""
    property var view: null
    property var viewWidth: 300
    property var anchorItem: null
    property int windowWidth: 300
    property int windowHeight: 500
    property bool contentScrolling: false
    required property DelegateModel delegateModel

    TextMetrics {
        id: textMetrics
        font.family: DTK.fontManager.baseFont.family
        font.pixelSize: DTK.fontManager.baseFont.pixelSize
    }

    function calculateMaxWidth() {
        if (!delegateModel || delegateModel.count === 0) return windowWidth

        var maxWidth = 0
        for (var i = 0; i < delegateModel.count; i++) {
            var item = delegateModel.items.get(i)
            if (item && item.model && item.model.display) {
                textMetrics.text = item.model.display
                maxWidth = Math.max(maxWidth, textMetrics.width)
            }
        }
        return Math.max(maxWidth + 50, 300)
    }

    signal opened()
    signal closed()
    
    // 设置视图索引的函数（无循环版本）
    function setViewIndex(viewIndex) {
        if (!view) return
        
        // 限制在有效范围内，不循环
        if (viewIndex < 0) {
            viewIndex = 0
        } else if (viewIndex >= view.count) {
            viewIndex = view.count - 1
        }
        
        view.currentIndex = viewIndex
        highlightedIndex = viewIndex
    }

    function show() {
        active = true
    }

    function isVisible() {
        return active
    }

    function close() {
        if (item) {
            item.closeWindow()
        }
    }

    function setPositionByItem(item) {
        anchorItem = item
        if (active && this.item) {
            this.item.positionWindow()
        }
    }

    sourceComponent: Window {
        id: searchWindow
        width: windowWidth
        height: loader.windowHeight
        minimumWidth: 300
        minimumHeight: loader.windowHeight
        maximumWidth: 500
        maximumHeight: loader.windowHeight
        DWindow.enabled: true
        DWindow.enableSystemResize: false
        DWindow.enableBlurWindow: true
        // ensure show in center of mainwindow
        flags: Qt.Dialog
        // default color is white
        color: DTK.palette.window
        palette: DTK.palette

        Component.onCompleted: {
            positionWindow()
            width = loader.calculateMaxWidth()
            loader.opened()
        }
        


        function positionWindow() {
            if (!loader.anchorItem) return

            var globalPos = loader.anchorItem.mapToGlobal(0, 0)
            searchWindow.x = globalPos.x
            searchWindow.y = globalPos.y + loader.anchorItem.height
        }

        ColumnLayout {
            id: contentLayout
            anchors.fill: parent
            anchors.margins: 6
            spacing: loader.delegateModel.count > 0 ? 14 : 0

            SearchEdit {
                id: searchEdit
                implicitHeight: 30
                Layout.fillWidth: true
                font: DTK.fontManager.t6
                Layout.alignment: Qt.AlignTop
                placeholder: qsTr("Search")
                onTextChanged: {
                    loader.searchText = text
                }
                onVisibleChanged: {
                    clear() // clear search text
                }
                
                // 键盘事件处理 - 回车直接选择
                Keys.onReturnPressed: {
                    if (listView.visible && listView.count > 0) {
                        // 如果没有选中项，默认选中第一项
                        if (listView.currentIndex < 0) {
                            loader.setViewIndex(0)
                        }
                        
                        // 直接设置checked状态触发选择
                        if (listView.currentIndex >= 0 && listView.currentItem) {
                            listView.currentItem.checked = true
                        }
                    }
                }
                
                Keys.onUpPressed: {
                    if (listView.visible && listView.count > 0) {
                        listView.forceActiveFocus()
                        // 如果没有选中项，选择最后一项，否则向上移动
                        if (listView.currentIndex < 0) {
                            loader.setViewIndex(listView.count - 1)
                        } else {
                            loader.setViewIndex(listView.currentIndex - 1)
                        }
                    }
                }
                
                Keys.onDownPressed: {
                    if (listView.visible && listView.count > 0) {
                        listView.forceActiveFocus()
                        // 如果没有选中项，选择第一项，否则向下移动
                        if (listView.currentIndex < 0) {
                            loader.setViewIndex(0)
                        } else {
                            loader.setViewIndex(listView.currentIndex + 1)
                        }
                    }
                }
            }

            // ArrowListView 样式的容器
            ColumnLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: Math.min(listView.contentHeight + (listView.interactive ? upButton.height + downButton.height : 0), 
                                                 loader.maxVisibleItems * 36 + (listView.interactive ? upButton.height + downButton.height : 0))
                visible: loader.delegateModel.count > 0
                spacing: 0
                
                // 上箭头按钮
                P.ArrowListViewButton {
                    id: upButton
                    visible: listView.interactive
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: width
                    Layout.preferredHeight: height
                    view: listView
                    direction: P.ArrowListViewButton.UpButton
                    focusPolicy: Qt.NoFocus
                    activeFocusOnTab: false
                }

                ListView {
                    id: listView
                    clip: true
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: loader.delegateModel
                    currentIndex: loader.highlightedIndex
                    highlightMoveDuration: -1
                    highlightMoveVelocity: -1
                    highlightFollowsCurrentItem: true
                    focus: true
                    activeFocusOnTab: true
                    
                    // 根据内容确定是否需要交互（模仿ArrowListView逻辑）
                    interactive: loader.delegateModel.count > loader.maxVisibleItems
                    
                    // 传递给delegate使用  
                    property bool keyboardScrolling: loader.contentScrolling
                    
                    // 键盘滚动保护计时器
                    Timer {
                        id: keyboardScrollTimer
                        interval: 50
                        onTriggered: {
                            loader.contentScrolling = false
                        }
                    }
                    
                    // 键盘事件处理（参考SearchBar模式）
                    Keys.onUpPressed: {
                        // 键盘导航时启用滚动保护
                        loader.contentScrolling = true
                        keyboardScrollTimer.restart()
                        loader.setViewIndex(currentIndex - 1)
                    }
                    
                    Keys.onDownPressed: {
                        // 键盘导航时启用滚动保护
                        loader.contentScrolling = true
                        keyboardScrollTimer.restart()
                        loader.setViewIndex(currentIndex + 1)
                    }
                    
                    Keys.onEscapePressed: {
                        // Escape键返回到搜索框
                        searchEdit.forceActiveFocus()
                    }

                    Component.onCompleted: {
                        loader.view = listView
                        loader.viewWidth = listView.width
                        
                        // 查找checked为true的项目并设置为当前高亮
                        var foundChecked = false
                        for (var i = 0; i < count; i++) {
                            var item = itemAtIndex(i)
                            if (item && item.checked === true) {
                                loader.setViewIndex(i)
                                positionViewAtIndex(i, ListView.Center)
                                foundChecked = true
                                break
                            }
                        }
                        
                        // 如果没找到checked项，使用highlightedIndex作为备选
                        if (!foundChecked && loader.highlightedIndex >= 0) {
                            loader.setViewIndex(loader.highlightedIndex)
                            positionViewAtIndex(loader.highlightedIndex, ListView.Center)
                        }
                        
                        // 确保获得焦点
                        forceActiveFocus()
                    }

                    onWidthChanged: {
                        loader.viewWidth = listView.width
                    }
                }
                
                // 下箭头按钮
                P.ArrowListViewButton {
                    id: downButton
                    visible: listView.interactive
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: width
                    Layout.preferredHeight: height
                    view: listView
                    direction: P.ArrowListViewButton.DownButton
                    enabled: !listView.atYEnd
                    focusPolicy: Qt.NoFocus
                    activeFocusOnTab: false
                }
            }

            Item {
                Layout.fillHeight: true
                visible: loader.delegateModel.count > 0
            }

            Text {
                id: noResultsText
                text: qsTr("No search results")
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                Layout.fillWidth: true
                Layout.fillHeight: true
                visible: loader.delegateModel.count === 0
                color: this.palette.windowText
                opacity: 0.4
                font: DTK.fontManager.t6
            }
        }

        function closeWindow() {
            searchEdit.clear()
            searchWindow.close()
            loader.active = false
            loader.closed()
        }

        onActiveFocusItemChanged: {
            if (!activeFocusItem) {
                searchWindow.closeWindow()
            }
        }

        onActiveChanged: {
            if (!active) {
                searchWindow.closeWindow()
            }
        }
    }

    onLoaded: {
        item.show()
        Qt.callLater(function() {
            item.requestActivate();
        });
    }
}
