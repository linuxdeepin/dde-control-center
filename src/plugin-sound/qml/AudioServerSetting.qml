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
        name: "audioServerTitle"
        parentName: "sound/audioServerSetting"
        displayName: qsTr("Audio Framework")
        description: qsTr("Different audio frameworks have their own advantages and disadvantages, and you can choose the one that best matches you to use")
        weight: 10
        pageType: DccObject.Item
        backgroundType: DccObject.AutoBg
        page: ColumnLayout {
            spacing: 2
            Label {
                Layout.alignment: Qt.AlignLeft
                Layout.leftMargin: 15
                font.pixelSize: 16
                font.bold: true
                text: dccObj.displayName
            }
            Label {
                Layout.alignment: Qt.AlignLeft
                font.pixelSize: 12
                Layout.leftMargin: 15
                text: dccObj.description
            }
        }
    }
    DccObject {
        name: "audioServerGrp"
        parentName: "sound/audioServerSetting"
        weight: 20
        pageType: DccObject.Item
        backgroundType: DccObject.Normal

        page: DeviceListView {
            enabled: dccData.model().audioServerStatus
            backgroundVisible: true
            showPlayBtn: false
            model: dccData.model().audioServerModel()
            onClicked: function (index, checked) {
                if (checked) {
                    dccData.worker().setAudioServerIndex(index)
                }
            }
        }
    }
}
