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
        name: "soundEffects"
        parentName: "sound/soundEffectsPage"
        displayName: qsTr("Sound Effects")
        weight: 10
        pageType: DccObject.Editor
        page: Switch {
            checked: dccData.model().enableSoundEffect
            onCheckedChanged: {
                dccData.worker().enableAllSoundEffect(checked)
            }
        }
    }
    DccObject {
        name: "effectsList"
        parentName: "sound/soundEffectsPage"
        weight: 20
        visible: dccData.model().enableSoundEffect
        pageType: DccObject.Item
        onParentItemChanged: item => { if (item) item.activeFocusOnTab = false }
        page: DeviceListView {
            backgroundVisible: false
            showPlayBtn: true
            model: dccData.model().soundEffectsModel()
            onClicked: function (index, checked) {
                dccData.worker().setSoundEffectEnable(index, checked)
            }

            onPlaybtnClicked: function (index) {
                console.log(" effectsList click play ", index)

                dccData.worker().playSoundEffect(index)
            }

            onVisibleChanged: {
                if (!visible) {
                    dccData.worker().stopSoundEffectPlayback();
                }
            }
        }
    }
}
