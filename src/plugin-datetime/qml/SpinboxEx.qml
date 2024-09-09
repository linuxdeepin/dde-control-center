// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.0

SpinBox {
    id: sp
    editable: true
    property string unitText
    implicitWidth: textMetrics.advanceWidth + 30

    TextMetrics {
        id: textMetrics
        text: sp.value + unitText + "LR"
    }

    Label {
        id: unit
        text: unitText
        anchors.right: parent.right
        anchors.rightMargin: 30
        height: parent.height
        verticalAlignment: Qt.AlignVCenter
    }
}
