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
            isGroup: false
        }
        onParentItemChanged: {
            if (parentItem) {
                parentItem.bottomInset = 5
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
            isGroup: false
        }
        onParentItemChanged: {
            if (parentItem) {
                parentItem.bottomInset = 10
            }
        }
        MicrophonePage {}
    }
    DccTitleObject{
        name: "soundSettings"
        parentName: "sound"
        displayName: qsTr("Settings")
        weight: 30
    }

    DccObject {
        name: "soundEffectsPage"
        parentName: "sound"
        displayName: qsTr("Sound Effects")
        description: qsTr("Enable/disable sound effects")
        icon: "system_sound"
        weight: 40
        page: DccRightView {
            isGroup: true
        }
        SoundEffectsPage {}
    }
    DccObject {
        name: "deviceManager"
        parentName: "sound"
        displayName: qsTr("Devices")
        description: qsTr("Enable/disable audio devices")
        icon: "equipment_management"
        visible: dccData.model().inPutPortCount !== 0 || dccData.model().outPutCount !== 0
        weight: 50
        SoundDevicemanagesPage {}
    }
}
