// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls 2.0
import QtQuick.Window
import org.deepin.dtk 1.0
import QtQuick.Layouts 1.0
import org.deepin.dtk.style 1.0 as DS

Loader {
    id: loader
    active: false
    required property var viewModel
    property int currentIndex: -1
    property string currentText
    property point mousePosition: Qt.point(0, 0)
    signal selectedRegion(string region)

    function show() {
        active = true
    }

    function isVisible() {
        return active
    }

    sourceComponent: Window {
        id: searchWindow
        property int calculatedWidth: 200
        width: calculatedWidth
        height: 500
        minimumWidth: width
        minimumHeight: height
        maximumWidth: minimumWidth
        maximumHeight: minimumHeight
        DWindow.enabled: true
        DWindow.enableSystemResize: false
        DWindow.enableBlurWindow: true
        // ensure show in center of mainwindow
        flags: Qt.Dialog
        // default color is white
        color: active ? DTK.palette.window : DTK.inactivePalette.window
        palette: DTK.palette

        TextMetrics {
            id: textMetrics
            font: DTK.fontManager.t6
            text: "" // Dynamically set text to measure actual width
        }

        function calculateOptimalWidth() {
            if (!viewModel || viewModel.rowCount() === 0) {
                calculatedWidth = 200
                return
            }

            var maxWidth = 0
            // Iterate through all items to find the longest text
            for (var i = 0; i < viewModel.rowCount(); i++) {
                var itemText = viewModel.data(viewModel.index(i, 0), Qt.DisplayRole) || ""
                if (itemText.length > 0) {
                    // Directly measure the actual width of the complete text
                    textMetrics.text = itemText
                    maxWidth = Math.max(maxWidth, textMetrics.advanceWidth)
                }
            }

            // Calculate final width: text width + margins + scrollbar + indicators etc.
            // Window margins 12px + MenuItem padding 20px + scrollbar 4px + buffer 4px = 40px
            var finalWidth = Math.max(200, maxWidth + 40)
            calculatedWidth = Math.min(finalWidth, 400) // Limit maximum width to 400px
        }

        Component.onCompleted: {
            calculateOptimalWidth()
        }

        Connections {
            target: viewModel
            function onModelReset() {
                calculateOptimalWidth()
            }
            function onRowsInserted() {
                calculateOptimalWidth()
            }
            function onRowsRemoved() {
                calculateOptimalWidth()
            }
        }

        ColumnLayout {
            id: contentLayout
            anchors.fill: parent
            anchors.margins: 6
            SearchEdit {
                id: searchEdit
                implicitHeight: Math.max(30, searchEditFontMetrics.height + (DS.Style.control.padding - DS.Style.control.borderWidth) * 2)
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop
                placeholder: qsTr("Search")
                palette: DTK.palette
                font: DTK.fontManager.t6
                onTextChanged: {
                    viewModel.setFilterWildcard(text);
                }
                onEditingFinished: {
                    viewModel.setFilterWildcard(text);
                }
                FontMetrics {
                    id: searchEditFontMetrics
                    font: searchEdit.font
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ArrowListView {
                    anchors.fill: parent
                    id: itemsView
                    property string checkedRegion
                    clip: true
                    maxVisibleItems: 12
                    view.model: viewModel
                    view.currentIndex: loader.currentIndex
                    view.ScrollBar.vertical: verticalScrollBar

                    ButtonGroup {
                        id: regionGroup
                    }

                view.delegate: MenuItem {
                    id: menuItem
                    implicitWidth: itemsView.width
                    implicitHeight: 30
                    text: model.display
                    checkable: true
                    checked: text === loader.currentText
                    hoverEnabled: true
                    highlighted: hovered
                    autoExclusive: true
                    ButtonGroup.group: regionGroup
                    useIndicatorPadding: true
                    font: DTK.fontManager.t6

                        onCheckedChanged: {
                            if (checked && loader.currentText !== model.display) {
                                selectedRegion(model.display)
                                closeWindow()
                            }
                        }
                    }

                    Component.onCompleted: {
                        // Set initial scroll position
                        if (currentIndex >= 0) {
                            let delegateHeight = 30
                            view.contentY = currentIndex * delegateHeight;
                        }
                    }
                }

                ScrollBar {
                    id: verticalScrollBar
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.rightMargin: -6
                    width: 10
                    orientation: Qt.Vertical

                    position: itemsView.view.visibleArea.yPosition
                    size: itemsView.view.visibleArea.heightRatio
                    active: hovered || pressed || itemsView.view.moving || itemsView.view.flicking
                }
            }
        }

        function closeWindow() {
            loader.viewModel.setFilterWildcard("");
            searchWindow.close()
            loader.active = false
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
        if (loader.mousePosition.x !== 0 || loader.mousePosition.y !== 0) {
            item.x = loader.mousePosition.x
            item.y = loader.mousePosition.y
        }
        Qt.callLater(item.requestActivate);
    }
}
