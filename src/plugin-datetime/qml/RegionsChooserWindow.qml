// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Lt
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts
import org.deepin.dtk 1.0
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dtk.private 1.0 as P

Popup {
    id: control
    width: calculatedWidth
    height: 500
    popupType: Popup.Window
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

    PopupHandle.enableBlurWindow: true

    required property var viewModel
    property int currentIndex: -1
    property string currentText
    property point mousePosition: Qt.point(0, 0)
    property int maxVisibleItems: 12
    signal selectedRegion(string region)

    function show() {
        if (mousePosition.x !== 0 || mousePosition.y !== 0) {
            var w = Window.window
            if (w) {
                var localPos = w.mapFromGlobal(mousePosition.x, mousePosition.y)
                x = localPos.x
                y = localPos.y
            }
        }
        calculateOptimalWidth()
        open()
    }

    function isVisible() {
        return visible
    }

    onClosed: {
        viewModel.setFilterWildcard("")
    }

    property int calculatedWidth: 200

    TextMetrics {
        id: textMetrics
        font: DTK.fontManager.t6
        text: ""
    }

    function calculateOptimalWidth() {
        if (!viewModel || viewModel.rowCount() === 0) {
            calculatedWidth = 200
            return
        }

        var maxWidth = 0
        for (var i = 0; i < viewModel.rowCount(); i++) {
            var itemText = viewModel.data(viewModel.index(i, 0), Qt.DisplayRole) || ""
            if (itemText.length > 0) {
                textMetrics.text = itemText
                maxWidth = Math.max(maxWidth, textMetrics.advanceWidth)
            }
        }

        var finalWidth = Math.max(200, maxWidth + 40)
        calculatedWidth = Math.min(finalWidth, 400)
    }

    Component.onCompleted: {
        calculateOptimalWidth()
    }

    Connections {
        target: viewModel
        function onModelReset() { calculateOptimalWidth() }
        function onRowsInserted() { calculateOptimalWidth() }
        function onRowsRemoved() { calculateOptimalWidth() }
    }

    contentItem: ColumnLayout {
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
            onTextChanged: { viewModel.setFilterWildcard(text) }
            onEditingFinished: { viewModel.setFilterWildcard(text) }
            FontMetrics {
                id: searchEditFontMetrics
                font: searchEdit.font
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

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

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ListView {
                    id: listView
                    anchors.fill: parent
                    clip: true
                    property string checkedRegion
                    model: viewModel
                    currentIndex: control.currentIndex
                    interactive: control.viewModel && control.viewModel.rowCount() > control.maxVisibleItems
                    ScrollBar.vertical: verticalScrollBar

                    ButtonGroup {
                        id: regionGroup
                    }

                    delegate: MenuItem {
                        id: menuItem
                        implicitWidth: listView.width
                        implicitHeight: 30
                        text: model.display
                        checkable: true
                        checked: text === control.currentText
                        hoverEnabled: true
                        highlighted: hovered
                        autoExclusive: true
                        ButtonGroup.group: regionGroup
                        useIndicatorPadding: true
                        font: DTK.fontManager.t6

                        onCheckedChanged: {
                            if (checked && control.currentText !== model.display) {
                                control.selectedRegion(model.display)
                                control.close()
                            }
                        }
                    }

                    Component.onCompleted: {
                        if (currentIndex >= 0) {
                            let delegateHeight = 30
                            contentY = currentIndex * delegateHeight
                        }
                    }
                }

                ScrollBar {
                    id: verticalScrollBar
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.rightMargin: -6
                }
            }

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
    }
}
