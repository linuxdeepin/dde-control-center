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
    property int windowHeight: 500
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

    function show() {
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
        if (w) {
            var localPos = w.mapFromGlobal(globalPos.x, globalPos.y)
            x = localPos.x
            y = localPos.y + anchorItem.height
        }
    }

    onClosed: {
        searchEdit.clear()
    }

    TextMetrics {
        id: textMetrics
        font.family: DTK.fontManager.baseFont.family
        font.pixelSize: DTK.fontManager.baseFont.pixelSize
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
            Layout.preferredHeight: Math.min(listView.contentHeight + (listView.interactive ? upButton.height + downButton.height : 0),
                                             control.maxVisibleItems * 36 + (listView.interactive ? upButton.height + downButton.height : 0))
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

            ListView {
                id: listView
                clip: true
                Layout.fillWidth: true
                Layout.fillHeight: true
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

                    var foundChecked = false
                    for (var i = 0; i < count; i++) {
                        var item = itemAtIndex(i)
                        if (item && item.checked === true) {
                            control.setViewIndex(i)
                            positionViewAtIndex(i, ListView.Center)
                            foundChecked = true
                            break
                        }
                    }

                    if (!foundChecked && control.highlightedIndex >= 0) {
                        control.setViewIndex(control.highlightedIndex)
                        positionViewAtIndex(control.highlightedIndex, ListView.Center)
                    }

                    forceActiveFocus()
                }

                onWidthChanged: {
                    control.viewWidth = listView.width
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

            ScrollBar {
                id: verticalScrollBar
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.rightMargin: -6
            }
        }

        Item {
            Layout.fillHeight: true
            visible: control.delegateModel.count > 0
        }

        Text {
            id: noResultsText
            text: qsTr("No search results")
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: control.delegateModel.count === 0
            color: this.palette.windowText
            opacity: 0.4
            font: DTK.fontManager.t6
        }
    }
}
