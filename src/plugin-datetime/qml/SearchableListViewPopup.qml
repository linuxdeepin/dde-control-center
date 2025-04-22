// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0
import org.deepin.dtk.style 1.0 as DS
Popup {
    id: control
    implicitWidth: 200
    implicitHeight: 496
    property int maxVisibleItems: 10
    property int highlightedIndex: 0
    property string searchText: searchEdit.text
    property alias view: arrowListView
    required property DelegateModel delegateModel

    Component.onCompleted: adjustPosition()

    contentItem: ColumnLayout {
        spacing: control.delegateModel.count > 0 ? 10 : 0
        Layout.fillWidth: true
        SearchEdit {
            id: searchEdit
            implicitHeight: 30
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            Layout.leftMargin: 0
            Layout.rightMargin: 0
            placeholder: qsTr("Search")
            onVisibleChanged: {
                clear() // clear seach text
            }
        }

        ArrowListView {
            id: arrowListView
            clip: true
            Layout.fillWidth: true
            visible: control.delegateModel.count  > 0
            maxVisibleItems: control.maxVisibleItems
            view.model: control.delegateModel
            view.currentIndex: control.highlightedIndex
            view.highlightMoveDuration: -1
            view.highlightMoveVelocity: -1
            view.onContentHeightChanged: Qt.callLater(adjustPosition)
        }

        Item {
            Layout.fillHeight: true
            visible: control.delegateModel.count  > 0
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
        }
    }

    function adjustPosition() {
        if (!parent) return

        // Get button position relative to current window
        var pos = parent.mapToItem(parent.Window.contentItem, 0, 0)
        // Calculate available space above and below
        var bottomSpace = parent.Window.height - (pos.y + parent.height)
        var topSpace = pos.y

        // Try to show below first
        if (bottomSpace >= height + 10) {
            // Enough space below, show below
            y = Math.min(parent.height + 5, bottomSpace - height - 10)
        } else if (topSpace >= height + 10) {
            // Not enough space below but enough space above, show above
            y = Math.max(-height, -topSpace + 10)
        } else {
            // Not enough space in either direction, show in direction with more space
            if (topSpace > bottomSpace) {
                y = Math.max(-height, -topSpace + 10)
            } else {
                y = Math.min(parent.height + 5, bottomSpace - height - 10)
            }
        }
    }
}
