// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

ItemDelegate {
    property alias isGroup: background.isGroup
    property alias separatorVisible: background.separatorVisible
    width: parent.width
    topInset: 5
    bottomInset: 5
    icon {
        name: model.item.icon
        width: 48
        height: 48
    }
    contentFlow: true
    content: RowLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        ColumnLayout {
            Label {
                text: model.display
                font: D.DTK.fontManager.t4
                elide: Text.ElideRight
            }
            Label {
                visible: text !== ""
                font: D.DTK.fontManager.t8
                elide: Text.ElideRight
                text: model.item.description
            }
        }
        D.IconLabel {
            Layout.alignment: Qt.AlignRight
            Layout.rightMargin: 10
            icon.name: "arrow_ordinary_right"
        }
    }
    background: DccListViewBackground {
        id: background
        separatorVisible: false
    }

    onClicked: {
        model.item.trigger()
        console.log(model.item.name, model.display, model.item.icon)
    }
}
