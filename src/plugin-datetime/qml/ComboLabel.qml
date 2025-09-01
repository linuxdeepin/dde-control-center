// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

Item {
    id: item
    property var comboModel: [""]
    property int comboCurrentIndex: -1
    // property string textRole
    implicitHeight: 30
    implicitWidth: 280
    signal comboBoxActivated(int index)

    RowLayout {
        anchors.fill: parent
        
        Item {
            Layout.fillWidth: true
        }
        
        ComboBox {
            id: comboBox
            visible: item.comboModel.length > 1
            flat: true
            implicitWidth: 220
            model: item.comboModel
            currentIndex: comboCurrentIndex
            hoverEnabled: true
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            onActivated: function (index) {
                item.comboBoxActivated(index)
            }
        }

        Label {
            id: label
            visible: item.comboModel.length === 1
            text: item.comboModel[0]
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.rightMargin: 10
        }
    }
}
