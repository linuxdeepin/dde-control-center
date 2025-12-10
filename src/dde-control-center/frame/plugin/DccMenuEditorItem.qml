// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D

DccEditorItem {
    id: control
    property Component editor: null
    leftPadding: 12
    rightPadding: 10
    topPadding: topInset
    bottomPadding: bottomInset
    activeFocusOnTab: true
    rightItem: RowLayout {
        spacing: 10
        DccLoader {
            active: !editor
            dccObj: model.item
            dccObjItem: control
        }
        Loader {
            active: editor
            sourceComponent: editor
        }
        D.IconLabel {
            icon {
                width: 12
                height: 12
                name: "arrow_ordinary_right"
                palette: D.DTK.makeIconPalette(control.palette)
                mode: control.D.ColorSelector.controlState
                theme: control.D.ColorSelector.controlTheme
            }
        }
    }
    onClicked: {
        if (model.item.children.length > 0) {
            DccApp.showPage(model.item.children[0])
        } else {
            console.warn(model.item.name, " MenuEditor nust include children", model.item.children.length)
        }
    }
}
