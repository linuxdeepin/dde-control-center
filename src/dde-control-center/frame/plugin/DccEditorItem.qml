// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

D.ItemDelegate {
    id: control
    property alias separatorVisible: background.separatorVisible
    property alias backgroundType: background.backgroundType
    property var item: model.item
    property Component rightItem: null
    property real iconRadius: model.item.iconRadius ? model.item.iconRadius : 0
    property real iconSize: model.item.iconSize ? model.item.iconSize : 0
    property real leftPaddingSize: model.item.leftPaddingSize ? model.item.leftPaddingSize : 10
    property real rightItemTopMargin: 5
    property real rightItemBottomMargin: 5

    implicitHeight: Math.max(model.item.description.length !== 0 ? 48 : 40, implicitContentHeight) + topInset + bottomInset
    backgroundVisible: false
    checkable: false
    topPadding: topInset
    bottomPadding: bottomInset
    leftPadding: control.leftPaddingSize
    rightPadding: 8
    hoverEnabled: model.item.enabledToApp
    checked: backgroundType & 0x08
    cascadeSelected: !checked
    font: D.DTK.fontManager.t6

    icon {
        name: model.item.icon
        source: model.item.iconSource
        width: control.iconSize > 0 ? control.iconSize : DS.Style.itemDelegate.iconSize
        height: control.iconSize > 0 ? control.iconSize : DS.Style.itemDelegate.iconSize
    }
    contentItem: RowLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true

        D.DciIcon {
            name: control.icon.name
            sourceSize: Qt.size(control.icon.width, control.icon.height)
            theme: D.DTK.themeType
            palette: D.DTK.makeIconPalette(control.palette)
        }
        ColumnLayout {
            Layout.leftMargin: model.item.icon.length === 0 ? 0 : 8
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            spacing: 2
            DccLabel {
                Layout.fillWidth: true
                text: model.display
                font: control.font
            }
            DccLabel {
                Layout.fillWidth: true
                visible: text !== ""
                font: D.DTK.fontManager.t10
                text: model.item.description
                opacity: 0.5
            }
        }
        DccLoader {
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.topMargin: control.rightItemTopMargin
            Layout.bottomMargin: control.rightItemBottomMargin
            enabled: model.item.enabledToApp
            opacity: enabled ? 1 : 0.4
            active: !rightItem
            dccObj: model.item
            dccObjItem: control
        }
        Loader {
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.topMargin: control.rightItemTopMargin
            Layout.bottomMargin: control.rightItemBottomMargin
            enabled: model.item.enabledToApp
            opacity: enabled ? 1 : 0.4
            active: rightItem
            sourceComponent: rightItem
        }
    }

    Loader {
        id: loader
        active: control.iconRadius > 0
        sourceComponent: D.ItemViewport {
            parent: control
            sourceItem: contentItem.children[0]
            radius: control.iconRadius
            fixed: true
            width: control.icon.width
            height: control.icon.height
            hideSource: true
            antialiasing: true
            x: 15
            y: (control.height - icon.height) / 2
        }
    }

    background: DccItemBackground {
        id: background
        separatorVisible: false
    }
    onClicked: {
        if ((backgroundType & 0x04) && model.item.enabledToApp) {
            model.item.active("")
        }
    }
}
