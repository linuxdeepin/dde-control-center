// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform
import Qt.labs.qmlmodels

import org.deepin.dtk as D
import org.deepin.dcc

RowLayout {
    objectName: "noPadding"
    Layout.fillWidth: true
    spacing: 10
    Repeater {
        id: repeater
        model: DccModel {
            root: dccObj
        }
        delegate: DccLoader {
            Layout.fillWidth: item ? item.Layout.fillWidth : false
            Layout.fillHeight: item ? item.Layout.fillHeight : false
            Layout.alignment: item ? item.Layout.alignment : Qt.AlignLeft
            dccObj: model.item
            dccObjItem: this
        }
    }
}
