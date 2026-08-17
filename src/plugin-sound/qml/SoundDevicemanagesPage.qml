// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

import org.deepin.dtk
import org.deepin.dcc

DccObject {
    DccTitleObject {
        name: "outputDevice"
        parentName: "sound/deviceManager"
        displayName: qsTr("Output Devices")
        description: qsTr("Select whether to enable the devices")
        visible: dccData.model().outPutCount !== 0
        weight: 10
    }
    DccObject {
        name: "outputDeviceList"
        parentName: "sound/deviceManager"
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
        onParentItemChanged: {
            if (parentItem) {
                parentItem.bottomInset = 15
            }
        }
    }
    DccTitleObject {
        name: "inputDevice"
        parentName: "sound/deviceManager"
        displayName: qsTr("Input Devices")
        description: qsTr("Select whether to enable the devices")
        visible: dccData.model().inPutPortCount !== 0
        weight: 30
    }
    DccObject {
        name: "inputDeviceList"
        parentName: "sound/deviceManager"
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
