// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0
import org.deepin.dcc 1.0

Rectangle {
    id: root
    property alias model: repeater.model
    signal clicked(int index, bool checked)

    color: "transparent"
    implicitHeight: layoutView.height
    Layout.fillWidth: true
    ColumnLayout {
        id: layoutView
        width: parent.width
        clip: true
        spacing: 0
        Repeater {
            id: repeater
            delegate: ItemDelegate {
                Layout.fillWidth: true
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                implicitHeight: 40
                cascadeSelected: true
                text: model.name
                content: IconButton {
                    icon.name: model.isChecked ? "qrc:/icons/deepin/builtin/actions/checked.png" : "qrc:/icons/deepin/builtin/actions/nocheck.png"
                    icon.width: 24
                    icon.height: 24
                    implicitWidth: 36
                    implicitHeight: 36
                    background: Rectangle {
                        color: "transparent" // 背景透明
                        border.color: "transparent" // 边框透明
                        border.width: 0
                    }
                    onClicked: {
                        root.clicked(index, !model.isChecked)
                    }
                }
                background: DccListViewBackground {
                    separatorVisible: true
                    highlightEnable: false
                }
            }
        }
    }
}
