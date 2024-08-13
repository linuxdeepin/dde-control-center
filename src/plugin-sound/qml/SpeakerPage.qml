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
        parentName: "sound/inAndOutPut"
        displayName: qsTr("输出")
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
        parentName: "sound/inAndOutPut"
        weight: 20
        pageType: DccObject.Item
        page: DccGroupView {}
        DccObject {
            name: "outputVolume"
            parentName: "sound/inAndOutPut/outputGroup"
            displayName: qsTr("输出音量")
            weight: 10
            pageType: DccObject.Editor
            // visible: outPutCombo.count > 0
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
            parentName: "sound/inAndOutPut/outputGroup"
            displayName: qsTr("音量增强")
            description: qsTr("音量大于100%时可能会导致音效失真,同时损害您的音频输出设备")
            weight: 20
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
            parentName: "sound/inAndOutPut/outputGroup"
            displayName: qsTr("音量平衡")
            weight: 30
            pageType: DccObject.Editor
            page: RowLayout {
                // spacing: 10
                Label {
                    Layout.alignment: Qt.AlignVCenter
                    font: DTK.fontManager.t7
                    Layout.topMargin: 2
                    text: "左"
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
                    text: "右"
                }
            }
        }
        DccObject {
            name: "plugAndUnplugManagement"
            parentName: "sound/inAndOutPut/outputGroup"
            displayName: qsTr("插拔管理")
            description: qsTr("外设插拔时音频输出是否自动暂停")
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
            parentName: "sound/inAndOutPut/outputGroup"
            displayName: qsTr("输出设备")
            weight: 50
            pageType: DccObject.Editor
            page: ComboBox {
                id: outPutCombo
                Layout.alignment: Qt.AlignRight
                Layout.rightMargin: 10
                flat: true
                currentIndex: dccData.model().outPutPortComboIndex
                model: dccData.model().outPutPortCombo
                // background: {
                //     color: "white"
                // }
                onCurrentIndexChanged: {
                    console.log("Selected index:", currentIndex)
                }
            }
        }
    }
}
