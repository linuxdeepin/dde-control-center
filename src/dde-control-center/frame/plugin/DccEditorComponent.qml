// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D

D.ItemDelegate {
    id: root
    property alias isGroup: background.isGroup
    property alias separatorVisible: background.separatorVisible
    property var item: model.item

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
            Label {
                text: model.display
                elide: Text.ElideRight
            }
            Label {
                visible: text !== ""
                font: D.DTK.fontManager.t8
                elide: Text.ElideRight
                text: model.item.description
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
    Component.onDestruction: {
        root.item.deactive()
    }
}
