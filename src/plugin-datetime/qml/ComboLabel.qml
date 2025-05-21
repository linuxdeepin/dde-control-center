// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls 2.0

Item {
    id: item
    property var comboModel: [""]
    property int comboCurrentIndex: -1
    // property string textRole
    implicitHeight: 30
    implicitWidth: item.comboModel.length > 1 ? 280 : 80
    signal comboBoxActivated(int index)

    ComboBox {
        id: comboBox
        visible: item.comboModel.length > 1
        flat: true
        implicitWidth: 280
        model: item.comboModel
        currentIndex: comboCurrentIndex
        hoverEnabled: true
        onActivated: function (index) {
            item.comboBoxActivated(index)
        }
        anchors {
            verticalCenter: parent.verticalCenter
        }
    }

    Label {
        id: label
        visible: item.comboModel.length === 1
        text: item.comboModel[0]
        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 10
        }
    }
}
