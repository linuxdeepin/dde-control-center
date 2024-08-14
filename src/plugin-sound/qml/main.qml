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
        displayName: qsTr("输出")
        icon: "system"
        weight: 10
        pageType: DccObject.Item
        page: DccGroupView {
            spacing: 5
            isGroup: false
            height: implicitHeight + 20
        }
        SpeakerPage {}
    }
    DccObject {
        name: "inPut"
        parentName: "sound"
        displayName: qsTr("输入")
        icon: "system"
        weight: 20
        pageType: DccObject.Item
        page: DccGroupView {
            spacing: 5
            isGroup: false
            height: implicitHeight + 20
        }
        MicrophonePage {}
    }
    DccObject {
        name: "soundEffectsPage"
        parentName: "sound"
        displayName: qsTr("系统音效")
        description: qsTr("控制系统声音效果")
        icon: "system"
        weight: 30
        page: DccRightView {
            isGroup: true
        }
        SoundEffectsPage {}
    }
    DccObject {
        name: "deviceManger"
        parentName: "sound"
        displayName: qsTr("设备管理")
        description: qsTr("选择是否启用设备")
        icon: "system"
        weight: 40
        SoundDevicemanagesPage {}
    }
}
