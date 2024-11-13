// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import org.deepin.dtk 1.0

import org.deepin.dcc 1.0

DccObject {
    id: root
    function toPercent(value: string) {
        return Number(value * 100).toFixed(0) + "%"
    }
    DccObject {
        name: "output"
        parentName: "sound/outPut"
        displayName: qsTr("Output")
        weight: 10
        hasBackground: false
        pageType: DccObject.Item
        page: ColumnLayout {
            Label {
                Layout.leftMargin: 10
                font: DTK.fontManager.t4
                text: dccObj.displayName
            }
        }
    }
    DccObject {
        name: "outputGroup"
        parentName: "sound/outPut"
        weight: 20
        pageType: DccObject.Item
        page: DccGroupView {}
        DccObject {
            name: "outputVolume"
            parentName: "sound/outPut/outputGroup"
            displayName: qsTr("Output Volume")
            weight: 10
            pageType: DccObject.Editor
            visible: dccData.model().outPutPortCombo.length !== 0
            page: RowLayout {
                Label {
                    font: DTK.fontManager.t7
                    text: root.toPercent(voiceTipsSlider.value)
                }
                IconButton {
                    Layout.alignment: Qt.AlignVCenter
                    icon.name: "qrc:/icons/deepin/builtin/actions/dcc_volume1_32px.svg"
                    icon.width: 24
                    icon.height: 24
                    implicitWidth: 24
                    background: Rectangle {
                        color: "transparent" // 背景透明
                        border.color: "transparent" // 边框透明
                        border.width: 0
                    }
                }
                Slider {
                    id: voiceTipsSlider
                    Layout.alignment: Qt.AlignVCenter
                    implicitHeight: 24
                    handleType: Slider.HandleType.NoArrowHorizontal
                    highlightedPassedGroove: true
                    value: dccData.model().speakerVolume
                    to: dccData.model().increaseVolume ? 1.5 : 1.0
                    onValueChanged: {
                        dccData.worker().setSinkVolume(voiceTipsSlider.value)
                    }
                }
                IconButton {
                    // Layout.alignment: Qt.AlignTop
                    icon.name: "qrc:/icons/deepin/builtin/actions/dcc_volume3_32px.svg"
                    icon.width: 24
                    icon.height: 24
                    implicitWidth: 24
                    background: Rectangle {
                        color: "transparent" // 背景透明
                        border.color: "transparent" // 边框透明
                        border.width: 0
                    }
                }
            }
        }
        DccObject {
            name: "volumeEnhancement"
            parentName: "sound/outPut/outputGroup"
            displayName: qsTr("Volume Boost")
            description: qsTr("If the volume is louder than 100%, it may distort audio and be harmful to output devices")
            weight: 20
            visible: dccData.model().outPutPortCombo.length !== 0
            pageType: DccObject.Editor
            page: Switch {
                Layout.alignment: Qt.AlignRight
                checked: dccData.model().increaseVolume

                onCheckedChanged: {
                    dccData.worker().setIncreaseVolume(checked)
                }
            }
        }
        DccObject {
            name: "volumeBalance"
            parentName: "sound/outPut/outputGroup"
            displayName: qsTr("Left Right Balance")
            weight: 30
            visible: dccData.model().outPutPortCombo.length !== 0
            pageType: DccObject.Editor
            page: RowLayout {
                Label {
                    Layout.alignment: Qt.AlignVCenter
                    font: DTK.fontManager.t7
                    Layout.topMargin: 2
                    text: qsTr("Left")
                }
                Slider {
                    id: balanceSlider
                    Layout.alignment: Qt.AlignVCenter
                    implicitHeight: 24
                    from: -1
                    handleType: Slider.HandleType.ArrowBottom
                    highlightedPassedGroove: true
                    stepSize: 0.00001
                    to: 1
                    value: dccData.model().speakerBalance

                    onValueChanged: {
                        dccData.worker().setSinkBalance(balanceSlider.value)
                    }
                }
                Label {
                    Layout.alignment: Qt.AlignVCenter
                    Layout.topMargin: 2
                    font: DTK.fontManager.t7
                    text: qsTr("Right")
                }
            }
        }
        DccObject {
            name: "plugAndUnplugManagement"
            parentName: "sound/outPut/outputGroup"
            displayName: qsTr("Auto pause")
            description: qsTr("Whether the audio will be automatically paused when the current audio device is unplugged")
            weight: 40
            pageType: DccObject.Editor
            page: Switch {
                checked: dccData.model().pausePlayer
                onCheckedChanged: {
                    dccData.worker().setPausePlayer(checked)
                }
            }
        }
        DccObject {
            name: "outputDevice"
            parentName: "sound/outPut/outputGroup"
            displayName: qsTr("Output Devices")
            weight: 50
            pageType: DccObject.Editor
            page: ComboBox {
                id: outPutCombo
                Layout.alignment: Qt.AlignRight
                Layout.rightMargin: 10
                flat: true
                currentIndex: dccData.model().outPutPortComboIndex
                model: dccData.model().outPutPortCombo

                property bool isInitialized: false
                // 等待组件加载完成后，设置 isInitialized 为 true
                Component.onCompleted: {
                    console.log("outputDevice onCompleted:", isInitialized)
                    isInitialized = true
                }
                onCurrentIndexChanged: {
                    console.log("Selected index:", currentIndex, isInitialized)
                    if (isInitialized) {
                        dccData.worker().setActivePort(currentIndex, 1)
                    }
                }
            }
        }
    }
}
