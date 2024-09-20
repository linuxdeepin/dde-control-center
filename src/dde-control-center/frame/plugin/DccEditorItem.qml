// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D

D.ItemDelegate {
    id: root
    property alias separatorVisible: background.separatorVisible
    property var item: model.item
    property var rightItem: model.item.getSectionItem()

    Layout.fillWidth: true
    backgroundVisible: false
    checkable: false
    enabled: model.item.enabledToApp
    topPadding: topInset
    bottomPadding: bottomInset
    leftPadding: 10
    rightPadding: 10
    icon {
        name: model.item.icon
        source: model.item.iconSource
    }
    contentFlow: true
    content: RowLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        ColumnLayout {
            Layout.leftMargin: 8
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            spacing: 0
            DccLabel {
                Layout.fillWidth: true
                text: model.display
            }
            DccLabel {
                Layout.fillWidth: true
                visible: text !== ""
                font: D.DTK.fontManager.t8
                text: model.item.description
                opacity: 0.5
            }
        }
        Control {
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.rightMargin: 10
            contentItem: rightItem
        }
    }
    background: DccListViewBackground {
        id: background
        separatorVisible: false
    }
    Component.onCompleted: {
        model.item.parentItem = root
    }
}
