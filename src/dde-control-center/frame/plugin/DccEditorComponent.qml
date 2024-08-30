// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D

D.ItemDelegate {
    id: root
    property bool isGroup: false
    property alias separatorVisible: background.separatorVisible
    property var item: model.item

    corners: isGroup ? getCornersForBackground(index, model.count) : D.RoundRectangle.TopLeftCorner | D.RoundRectangle.TopRightCorner | D.RoundRectangle.BottomLeftCorner | D.RoundRectangle.BottomRightCorner
    Layout.fillWidth: true
    backgroundVisible: false
    checkable: false
    enabled: model.item.enabledToApp
    icon.name: model.item.icon
    contentFlow: true
    content: RowLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        ColumnLayout {
            Layout.fillWidth: true
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
            Layout.alignment: Qt.AlignRight
            Layout.rightMargin: 10
            contentItem: model.item.getSectionItem()
        }
    }
    background: DccListViewBackground {
        id: background
        separatorVisible: false
    }
    Component.onCompleted: {
        model.item.anchorsItem = root
    }
}
