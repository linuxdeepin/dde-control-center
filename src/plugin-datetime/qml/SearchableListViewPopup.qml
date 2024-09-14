// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0

Popup {
    id: control
    implicitWidth: 200
    property int maxVisibleItems: 10
    property int highlightedIndex: 0
    property string searchText: searchEdit.text
    property alias view: arrowListView
    required property DelegateModel delegateModel

    contentItem: ColumnLayout {
        spacing: 10
        Layout.fillWidth: true
        SearchEdit {
            id: searchEdit
            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            placeholder: qsTr("搜索")
            onVisibleChanged: {
                clear() // clear seach text
            }
        }

        ArrowListView {
            id: arrowListView
            clip: true
            Layout.fillWidth: true
            maxVisibleItems: control.maxVisibleItems
            view.model: control.delegateModel
            view.currentIndex: control.highlightedIndex
            view.highlightRangeMode: ListView.ApplyRange
            view.highlightMoveDuration: 0
        }
    }
}
