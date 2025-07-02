// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window
import org.deepin.dtk 1.0
import org.deepin.dtk.style 1.0 as DS

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
        color: active ? DTK.palette.window : DTK.inactivePalette.window

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
                Layout.alignment: Qt.AlignTop
                placeholder: qsTr("Search")
                onTextChanged: {
                    loader.searchText = text
                }
                onVisibleChanged: {
                    clear() // clear search text
                }
            }

            ArrowListView {
                id: arrowListView
                clip: true
                Layout.fillWidth: true
                visible: loader.delegateModel.count > 0
                maxVisibleItems: loader.maxVisibleItems
                view.model: loader.delegateModel
                view.currentIndex: loader.highlightedIndex
                view.highlightMoveDuration: -1
                view.highlightMoveVelocity: -1

                Component.onCompleted: {
                    loader.view = arrowListView
                    loader.viewWidth = arrowListView.width
                }

                onWidthChanged: {
                    loader.viewWidth = arrowListView.width
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
