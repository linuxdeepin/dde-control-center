// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import org.deepin.dtk 1.0

import org.deepin.dcc 1.0

DccObject {
    id: root

    // readonly property int itemDelegateMaxWidth: parent.width
    // readonly property int itemDelegateMaxheight: 40

    function toPercent(value: string) {
        return Number(value * 100).toFixed(0) + "%"
    }

    DccObject {
        name: "inPut"
        parentName: "sound/inPut"
        displayName: qsTr("输入")
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
        name: "inputGroup"
        parentName: "sound/inPut"
        weight: 20
        pageType: DccObject.Item
        page: DccGroupView {}
        DccObject{
            name: "inputVolume"
            parentName: "sound/inPut/inputGroup"
            displayName: qsTr("输入音量")
            weight: 10
            pageType: DccObject.Editor
            //visible: inPutCombo.count > 0
            page: RowLayout {
                Layout.alignment: Qt.AlignRight

                Label {
                    Layout.alignment: Qt.AlignVCenter
                    font: DTK.fontManager.t7
                    text: root.toPercent(voiceTipsSlider1.value)
                }

                IconButton {
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
                    id: voiceTipsSlider1
                    Layout.alignment: Qt.AlignVCenter
                    implicitHeight: 24
                    handleType: Slider.HandleType.NoArrowHorizontal
                    highlightedPassedGroove: true
                    from: 0
                    stepSize: 0.00001
                    to: 1
                    value: dccData.model().microphoneVolume
                   // Layout.topMargin: 12
                    onValueChanged: {
                        dccData.worker().setSourceVolume(voiceTipsSlider1.value)
                    }
                }
                IconButton {
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
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

        DccObject{
            name: "microphoneFeedback"
            parentName: "sound/inPut/inputGroup"
            displayName: qsTr("反馈音量")
            weight: 20
            pageType: DccObject.Editor
            visible: inPutCombo.count > 0
            page: RowLayout {
                Layout.alignment: Qt.AlignRight
                Layout.rightMargin: 10
                spacing: 10

                DciIcon {
                    name: "entry_voice"
                    sourceSize: Qt.size(16, 16)
                }
                Slider {
                    Layout.fillWidth: false
                    handleType: Slider.HandleType.NoArrowHorizontal
                    highlightedPassedGroove: true
                    implicitHeight: 24
                    value: dccData.model().microphoneFeedback
                }
                DciIcon {
                    name: "entry_voice"
                    sourceSize: Qt.size(16, 16)
                }
            }
        }

        DccObject {
            name: "reduceNoise"
            parentName: "sound/inPut/inputGroup"
            displayName: qsTr("噪音抑制")
            weight: 30
            pageType: DccObject.Editor
            page: Switch {
                Layout.alignment: Qt.AlignRight | Qt.AlignTop

                checked: dccData.model().reduceNoise
                onCheckedChanged: {
                    dccData.worker().setReduceNoise(checked)
                }
            }
        }

        DccObject {
            name: "inputDevice"
            parentName: "sound/inPut/inputGroup"
            displayName: qsTr("输入设备")
            weight: 40
            pageType: DccObject.Editor
            page: ComboBox {
                id: inPutCombo
                Layout.alignment: Qt.AlignRight
                Layout.rightMargin: 10
                currentIndex: dccData.model().inPutPortComboIndex
                flat: true
                model: dccData.model().inPutPortCombo
                onCurrentIndexChanged: {
                    console.log("inputDevice  Selected index:", currentIndex)
                }
            }
        }
    }
}
