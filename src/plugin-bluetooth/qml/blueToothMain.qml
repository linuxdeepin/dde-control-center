// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0

import org.deepin.dcc 1.0
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import Qt.labs.qmlmodels 1.2

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
                spacing: 0
                isGroup: false
            }

            Adapters{}
        }
    }
}
