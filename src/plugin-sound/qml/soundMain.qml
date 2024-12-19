// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    DccObject {
        name: "outPut"
        parentName: "sound"
        displayName: qsTr("Output")
        icon: "system"
        weight: 10
        pageType: DccObject.Item
        page: DccGroupView {
            spacing: 5
            isGroup: false
        }
        onParentItemChanged: {
            if (parentItem) {
                parentItem.bottomPadding = 20
            }
        }
        SpeakerPage {}
    }
    DccObject {
        name: "inPut"
        parentName: "sound"
        displayName: qsTr("Input")
        icon: "system"
        weight: 20
        pageType: DccObject.Item
        page: DccGroupView {
            spacing: 5
            isGroup: false
        }
        onParentItemChanged: {
            if (parentItem) {
                parentItem.bottomPadding = 20
            }
        }
        MicrophonePage {}
    }
    DccObject {
        name: "soundEffectsPage"
        parentName: "sound"
        displayName: qsTr("Sound Effects")
        description: qsTr("Enable/disable sound effects")
        icon: "system_sound"
        weight: 30
        page: DccRightView {
            isGroup: true
        }
        SoundEffectsPage {}
    }
    DccObject {
        name: "deviceManger"
        parentName: "sound"
        displayName: qsTr("DevicesManger")
        description: qsTr("Enable/disable audio devices")
        icon: "equipment_management"
        visible: dccData.model().inPutPortCount !== 0 || dccData.model().outPutCount !== 0
        weight: 40
        SoundDevicemanagesPage {}
    }
}
