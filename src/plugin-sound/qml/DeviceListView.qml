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
    property bool backgroundVisible: true
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
                leftPadding: 10
                rightPadding: 10
                implicitHeight: 40
                cascadeSelected: true
                backgroundVisible: root.backgroundVisible
                text: model.name
                content: DccCheckIcon {
                    checked: model.isChecked
                    size: 16
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
