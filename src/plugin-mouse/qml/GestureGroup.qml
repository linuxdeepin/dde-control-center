// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D
import org.deepin.dcc 1.0

Rectangle {
    id: root
    property alias model: repeater.model
    property bool backgroundVisible: true
    property bool showPlayBtn: false
    signal clicked(int index, bool checked)
    signal playbtnClicked(int index)

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
            delegate: D.ItemDelegate {
                id: itemCtl
                Layout.fillWidth: true
                leftPadding: 10
                rightPadding: 10
                implicitHeight: 45
                cascadeSelected: true
                backgroundVisible: root.backgroundVisible
                text: model.name
                icon.name: model.iconName

                hoverEnabled: true
                content: D.ComboBox {
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    model: [qsTr("Scroll Page")]
                    currentIndex: 0
                }
                background: DccItemBackground {
                    separatorVisible: true
                   // highlightEnable: false
                }              

                onClicked: {
                    root.clicked(index, !model.isChecked)
                }
            }
        }
    }
}