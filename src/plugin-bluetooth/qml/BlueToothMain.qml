// SPDX-FileCopyrightText: 2024-2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls

import org.deepin.dcc
import QtQuick.Layouts
import Qt.labs.platform
import Qt.labs.qmlmodels

DccObject{
    name: "bluetoothSetting"
    parentName: "bluetooth"
    weight: 10
    pageType: DccObject.Item
    page: DccGroupView {
        spacing: 0
        isGroup: false
    }
    DccRepeater {
        model: dccData.model().blueToothAdaptersModel()

        delegate: DccObject {
            name: "blueToothAdapters" + model.id + index
            parentName: "bluetoothSetting"
            weight: 10
            pageType: DccObject.Item

            page: DccGroupView {
                spacing: 10
                isGroup: false
            }

            Adapters{}
        }
    }
}
