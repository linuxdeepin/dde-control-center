// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

D.ItemDelegate {
    id: root
    property alias separatorVisible: background.separatorVisible
    property var item: model.item
    property var rightItem: null
    property real iconRadius: model.item.iconRadius ? model.item.iconRadius : 0
    property real iconSize: model.item.iconSize ? model.item.iconSize : 0

    Layout.fillWidth: true
    backgroundVisible: false
    checkable: false
    enabled: model.item.enabledToApp
    topPadding: topInset
    bottomPadding: bottomInset
    leftPadding: 10
    rightPadding: 8
    icon {
        name: model.item.icon
        source: model.item.iconSource
        width: root.iconSize > 0 ? root.iconSize : DS.Style.itemDelegate.iconSize
        height: root.iconSize > 0 ? root.iconSize : DS.Style.itemDelegate.iconSize
    }
    contentFlow: true
    content: RowLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        ColumnLayout {
            Layout.leftMargin: model.item.icon.length === 0 ? 0 : 8
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
            Layout.topMargin: 5
            Layout.bottomMargin: 5
            contentItem: rightItem ? rightItem : model.item.getSectionItem(this)
        }
    }

    Loader {
        id: loader
        active: root.iconRadius > 0
        sourceComponent: D.ItemViewport {
            parent: root
            sourceItem: contentItem.children[0]
            radius: root.iconRadius
            fixed: true
            width: root.icon.width
            height: root.icon.height
            hideSource: true
            antialiasing: true
            x: 15
            y: (root.height - icon.height) / 2
        }
    }

    background: DccItemBackground {
        id: background
        separatorVisible: false
    }
    Component.onCompleted: {
        model.item.parentItem = root
    }
}
