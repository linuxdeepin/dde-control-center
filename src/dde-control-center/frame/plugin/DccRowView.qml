// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import Qt.labs.qmlmodels 1.2

import org.deepin.dtk 1.0 as D
import org.deepin.dcc 1.0

RowLayout {
    id: root
    objectName: "noPadding"
    Layout.fillWidth: true
    spacing: 10
    Repeater {
        id: repeater
        model: DccModel {
            root: dccObj
        }
        delegate: Control {
            Layout.fillWidth: contentItem.Layout.fillWidth
            Layout.alignment: contentItem.Layout.alignment
            contentItem: model.item.getSectionItem(this)
            Component.onCompleted: {
                model.item.parentItem = this
            }
        }
    }
}
