// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import org.deepin.dtk 1.0
import org.deepin.dcc 1.0

DccObject {
    DccObject {
        name: "outputDevice"
        parentName: "sound/deviceManger"
        displayName: qsTr("Output Devices")
        description: qsTr("Select whether to enable the devices")
        visible: dccData.model().outPutCount !== 0
        weight: 10
        pageType: DccObject.Editor
    }
    DccObject {
        name: "outputDeviceList"
        parentName: "sound/deviceManger"
        weight: 20
        visible: dccData.model().outPutCount !== 0
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: DeviceListView {
            model: dccData.model().soundOutputDeviceModel()
            onClicked: function (index, checked) {
                dccData.worker().setPortEnableIndex(index, checked, 1)
            }
        }
    }
    DccObject {
        name: "inputDevice"
        parentName: "sound/deviceManger"
        displayName: qsTr("Input Devices")
        description: qsTr("Select whether to enable the devices")
        visible: dccData.model().inPutPortCount !== 0
        weight: 30
        pageType: DccObject.Editor
    }
    DccObject {
        name: "inputDeviceList"
        parentName: "sound/deviceManger"
        weight: 40
        visible: dccData.model().inPutPortCount !== 0
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: DeviceListView {
            model: dccData.model().soundInputDeviceModel()
            onClicked: function (index, checked) {
                dccData.worker().setPortEnableIndex(index, checked, 2)
            }
        }
    }
}
