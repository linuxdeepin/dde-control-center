// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import org.deepin.dtk 1.0 as D

SpinBox {
    id: sp
    editable: true
    property string unitText
    implicitWidth: valueMetrics.width + unitMetrics.width + 60

    TextMetrics {
        id: valueMetrics
        text: Math.max(sp.from, sp.to).toString()
    }

    TextMetrics {
        id: unitMetrics
        text: unitText
    }

    Label {
        id: unit
        text: unitText
        font: D.DTK.fontManager.t6
        anchors.right: parent.right
        anchors.rightMargin: 30
        height: parent.height
        verticalAlignment: Qt.AlignVCenter
    }
}
