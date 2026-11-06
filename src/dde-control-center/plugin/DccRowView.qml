// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import Qt.labs.qmlmodels 1.2

import org.deepin.dtk 1.0 as D
import org.deepin.dcc 1.0

RowLayout {
    objectName: "noPadding"
    Layout.fillWidth: true
    clip: true
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
            Layout.leftMargin: item ? item.Layout.leftMargin : 0
            Layout.rightMargin: item ? item.Layout.rightMargin : 0
            Layout.topMargin: item ? item.Layout.topMargin : 0
            Layout.bottomMargin: item ? item.Layout.bottomMargin : 0
            dccObj: model.item
            dccObjItem: this
        }
    }
}
