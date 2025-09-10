// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0

import org.deepin.dcc 1.0
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0

DccObject{
    DccObject {
        name: "myDeviceTitle"
        parentName: "myDevice" + model.name
        displayName: qsTr("My Devices")
        weight: 10
        pageType: DccObject.Item
        visible: model.myDeviceVisiable
        page: Label {
            leftPadding: 10
            font.bold: true
            font.pixelSize: DTK.fontManager.t5.pixelSize
            text: dccObj.displayName
        }

    }

    DccObject {
        name: "myDeviceList"
        parentName: "myDevice" + model.name
        weight: 11
        backgroundType: DccObject.Normal
        visible: model.myDeviceVisiable
        pageType: DccObject.Item
        page: BlueToothDeviceListView {
            deviceModel: model.myDevice
            onClicked: function (index, checked) {
            }
        }
    }
}
