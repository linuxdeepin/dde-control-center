// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Lt
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import org.deepin.dtk 1.0
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dtk.private 1.0 as P

Popup {
    id: control
    width: windowWidth
    implicitWidth: windowWidth
    height: windowHeight
    popupType: Popup.Window
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

    PopupHandle.enableBlurWindow: true

    property int maxVisibleItems: 10
    property int highlightedIndex: 0
    property string searchText: ""
    property var view: null
    property var viewWidth: 300
    property var anchorItem: null
    property int windowWidth: 300
    // Stable default height for the no-result placeholder so the menu keeps a
    // consistent "default" height and the hint text can be centered (PMS #373823).
    readonly property int noResultAreaHeight: control.maxVisibleItems * 36
    // Height of the list area: capped at maxVisibleItems, scrolling otherwise.
    readonly property int listAreaHeight: control.delegateModel.count > 0
        ? Math.min(listView.contentHeight, control.maxVisibleItems * 36)
          + (listView.interactive ? (upButton.height + downButton.height) : 0)
        : control.noResultAreaHeight
    // Content-driven popup height. Equals contentItem implicit height + insets,
    // so it stays correct whether the Popup window honors `height` directly or
    // sizes itself to the contentItem (PMS #373843, #373823).
    property int windowHeight: 2 * (control.padding + 6)
        + searchEdit.implicitHeight
        + (control.delegateModel.count > 0 ? 14 : 0)
        + control.listAreaHeight
    // Conservative upper bound used only for screen-bounds clamping, so the menu
    // is positioned safely even before the list finishes laying out.
    readonly property int maxClampHeight: 2 * (control.padding + 6)
        + searchEdit.implicitHeight + 14
        + control.maxVisibleItems * 36 + 80
    property bool contentScrolling: false
    required property DelegateModel delegateModel

    function setViewIndex(viewIndex) {
        if (!view) return

        if (viewIndex < 0) {
            viewIndex = 0
        } else if (viewIndex >= view.count) {
            viewIndex = view.count - 1
        }

        view.currentIndex = viewIndex
        highlightedIndex = viewIndex
    }

    function scrollToHighlighted() {
        if (!view) return

        var foundChecked = false
        for (var i = 0; i < view.count; i++) {
            var item = view.itemAtIndex(i)
            if (item && item.checked === true) {
                setViewIndex(i)
                view.positionViewAtIndex(i, ListView.Center)
                foundChecked = true
                break
            }
        }

        if (!foundChecked && highlightedIndex >= 0) {
            setViewIndex(highlightedIndex)
            view.positionViewAtIndex(highlightedIndex, ListView.Center)
        }
    }

    // Fit the popup width to the widest entry of the (source) model so timezone
    // names are no longer elided, restoring the content-based width removed for
    // performance in 7bfbe1fb0 (PMS #373843). Measuring model data via
    // TextMetrics avoids instantiating delegates and stays cheap.
    function calculateOptimalWidth() {
        var proxyModel = control.delegateModel && control.delegateModel.model
                         ? control.delegateModel.model : null
        var srcModel = proxyModel && proxyModel.sourceModel
                       ? proxyModel.sourceModel : proxyModel
        if (!srcModel || srcModel.rowCount() === 0) {
            control.windowWidth = 300
            return
        }
        var maxWidth = 0
        for (var i = 0; i < srcModel.rowCount(); i++) {
            var txt = srcModel.data(srcModel.index(i, 0), Qt.DisplayRole)
            if (txt && txt.length > 0) {
                textMetrics.text = txt
                maxWidth = Math.max(maxWidth, textMetrics.advanceWidth)
            }
        }
        // +indicator(20) +spacing(8) +scrollbar/menu padding; cap [300, 500]
        control.windowWidth = Math.min(Math.max(maxWidth + 2 * (control.padding + 6) + 36, 300), 500)
    }

    function show() {
        calculateOptimalWidth()
        if (anchorItem) {
            positionWindow()
        }
        open()
    }

    function isVisible() {
        return visible
    }

    function setPositionByItem(item) {
        anchorItem = item
        if (visible) {
            positionWindow()
        }
    }

    function positionWindow() {
        if (!anchorItem) return

        var globalPos = anchorItem.mapToGlobal(0, 0)
        var w = Window.window
        if (!w) return

        var popupW = control.width
        // Clamp with the max possible height so the menu stays on screen even
        // before the list finishes laying out.
        var popupH = Math.max(control.height, control.maxClampHeight)

        var posX = globalPos.x
        var posY = globalPos.y + anchorItem.height

        // QML's Screen attached object does not expose availableGeometry, so
        // derive a taskbar-aware rect from it (falls back to the full screen
        // when no panel space is reserved). Keep the menu 10px clear of the
        // taskbar / screen edge (PMS #373843).
        var availLeft = Screen.virtualX
        var availTop = Screen.virtualY
        var availRight = Screen.virtualX + Screen.width
        var availBottom = Screen.virtualY + Screen.height
        if (Screen.desktopAvailableHeight > 0
                && Screen.desktopAvailableHeight < Screen.height)
            availBottom = Screen.virtualY + Screen.desktopAvailableHeight
        if (Screen.desktopAvailableWidth > 0
                && Screen.desktopAvailableWidth < Screen.width)
            availRight = Screen.virtualX + Screen.desktopAvailableWidth

        var margin = 10
        if (posX + popupW > availRight - margin)
            posX = availRight - popupW - margin
        if (posX < availLeft + margin)
            posX = availLeft + margin
        if (posY + popupH > availBottom - margin)
            posY = availBottom - popupH - margin
        if (posY < availTop + margin)
            posY = availTop + margin

        var localPos = w.mapFromGlobal(posX, posY)
        x = localPos.x
        y = localPos.y
    }

    onClosed: {
        searchEdit.clear()
    }
    
    onOpened: {
        scrollToHighlighted()
        if (view) view.forceActiveFocus()
    }


    TextMetrics {
        id: textMetrics
        font: DTK.fontManager.t6
    }

    contentItem: ColumnLayout {
        anchors.fill: parent
        anchors.margins: 6
        spacing: control.delegateModel.count > 0 ? 14 : 0

        SearchEdit {
            id: searchEdit
            implicitHeight: Math.max(30, searchEditFontMetrics.height + (DS.Style.control.padding - DS.Style.control.borderWidth) * 2)
            Layout.fillWidth: true
            font: DTK.fontManager.t6
            Layout.alignment: Qt.AlignTop
            placeholder: qsTr("Search")
            Timer {
                id: searchDebounceTimer
                interval: 100
                onTriggered: {
                    control.searchText = searchEdit.text
                }
            }

            onTextChanged: {
                searchDebounceTimer.restart()
            }
            onVisibleChanged: {
                clear()
            }
            FontMetrics {
                id: searchEditFontMetrics
                font: searchEdit.font
            }

            Keys.onReturnPressed: {
                if (listView.visible && listView.count > 0) {
                    if (listView.currentIndex < 0) {
                        control.setViewIndex(0)
                    }

                    if (listView.currentIndex >= 0 && listView.currentItem) {
                        listView.currentItem.checked = true
                    }
                }
            }

            Keys.onUpPressed: {
                if (listView.visible && listView.count > 0) {
                    listView.forceActiveFocus()
                    if (listView.currentIndex < 0) {
                        control.setViewIndex(listView.count - 1)
                    } else {
                        control.setViewIndex(listView.currentIndex - 1)
                    }
                }
            }

            Keys.onDownPressed: {
                if (listView.visible && listView.count > 0) {
                    listView.forceActiveFocus()
                    if (listView.currentIndex < 0) {
                        control.setViewIndex(0)
                    } else {
                        control.setViewIndex(listView.currentIndex + 1)
                    }
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: control.listAreaHeight
            visible: control.delegateModel.count > 0
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
                    clip: true
                    anchors.fill: parent
                    model: control.delegateModel
                    currentIndex: control.highlightedIndex
                    highlightMoveDuration: -1
                    highlightMoveVelocity: -1
                    highlightFollowsCurrentItem: true
                    focus: true
                    activeFocusOnTab: true
                    ScrollBar.vertical: verticalScrollBar

                    interactive: control.delegateModel.count > control.maxVisibleItems

                    property bool keyboardScrolling: control.contentScrolling

                    Timer {
                        id: keyboardScrollTimer
                        interval: 50
                        onTriggered: {
                            control.contentScrolling = false
                        }
                    }

                    Keys.onUpPressed: {
                        control.contentScrolling = true
                        keyboardScrollTimer.restart()
                        control.setViewIndex(currentIndex - 1)
                    }

                    Keys.onDownPressed: {
                        control.contentScrolling = true
                        keyboardScrollTimer.restart()
                        control.setViewIndex(currentIndex + 1)
                    }

                    Keys.onEscapePressed: {
                        searchEdit.forceActiveFocus()
                    }

                    Component.onCompleted: {
                        control.view = listView
                        control.viewWidth = listView.width
                        control.scrollToHighlighted()
                        forceActiveFocus()
                    }

                    onWidthChanged: {
                        control.viewWidth = listView.width
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

        // No-result placeholder: a stable default-height area with the hint
        // text centered, so the menu no longer collapses on no matches and the
        // hint is vertically centered (PMS #373823, #373843).
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: control.noResultAreaHeight
            visible: control.delegateModel.count === 0

            Text {
                anchors.centerIn: parent
                text: qsTr("No search results")
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: this.palette.windowText
                opacity: 0.4
                font: DTK.fontManager.t6
            }
        }
    }
}
