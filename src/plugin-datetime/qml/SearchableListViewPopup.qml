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
    property int maxVisibleItems: 10
    property int highlightedIndex: 0
    property string searchText: searchEdit.text
    property alias view: arrowListView
    required property DelegateModel delegateModel

    Component.onCompleted: adjustPosition()

    contentItem: ColumnLayout {
        spacing: 10
        Layout.fillWidth: true
        SearchEdit {
            id: searchEdit
            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            placeholder: qsTr("Search")
            onVisibleChanged: {
                clear() // clear seach text
            }
        }

        ArrowListView {
            id: arrowListView
            clip: true
            Layout.fillWidth: true
            visible: view.count > 0
            maxVisibleItems: control.maxVisibleItems
            view.model: control.delegateModel
            view.currentIndex: control.highlightedIndex
            view.highlightMoveDuration: -1
            view.highlightMoveVelocity: -1
            view.onContentHeightChanged: Qt.callLater(adjustPosition)
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
