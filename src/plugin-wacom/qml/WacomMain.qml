// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

import org.deepin.dcc 1.0

DccObject {
    DccObject {
        name: "wacomTitle"
        parentName: "wacom"
        displayName: qsTr("wacom")
        weight: 10
        pageType: DccObject.Item
        page: ColumnLayout {
            Label {
                height: contentHeight
                Layout.leftMargin: 10
                font.family: D.DTK.fontManager.t5.family
                font.bold: true
                font.pixelSize: D.DTK.fontManager.t5.pixelSize
                text: dccObj.displayName
            }
        }
    }

    DccObject {
        name: "mode"
        parentName: "wacom"
        displayName: qsTr("Model")
        weight: 20
        backgroundType: DccObject.Normal
        pageType: DccObject.Editor
        page: ComboBox {
            model: [qsTr("Pen Mode"), qsTr("Mouse Mode")]
            currentIndex: dccData.CursorMode
            onCurrentIndexChanged: {
                dccData.CursorMode = currentIndex
            }
        }
    }

    DccObject {
        name: "pressure"
        parentName: "wacom"
        displayName: qsTr("Pressure Sensitivity")
        backgroundType: DccObject.Normal
        weight: 30
        pageType: DccObject.Item
        visible: !dccData.CursorMode
        page: ColumnLayout {
            Layout.fillHeight: true
            Label {
                id: speedText
                Layout.topMargin: 10
                font: D.DTK.fontManager.t6
                text: dccObj.displayName
                Layout.leftMargin: 20
            }
            D.TipsSlider {
                id: scrollSlider
                readonly property var tips: [qsTr("Light"), (""), (""), (""), (""), (""), qsTr("Heavy")]

                property real leftTextWidth: 0
                property real rightTextWidth: 0
                property real dynamicLeftMargin: Math.max(16, leftTextWidth / 2 + 5)
                property real dynamicRightMargin: Math.max(16, rightTextWidth / 2 + 5)

                Layout.preferredHeight: 90
                Layout.alignment: Qt.AlignCenter
                Layout.leftMargin: dynamicLeftMargin
                Layout.rightMargin: dynamicRightMargin
                Layout.topMargin: 10
                Layout.bottomMargin: 10
                Layout.fillWidth: true

                TextMetrics {
                    id: scrollLeftTextMetrics
                    font: scrollSlider.slider.font
                    text: scrollSlider.tips[0]
                    Component.onCompleted: {
                        scrollSlider.leftTextWidth = width
                    }
                }
                TextMetrics {
                    id: scrollRightTextMetrics
                    font: scrollSlider.slider.font
                    text: scrollSlider.tips[scrollSlider.tips.length - 1]
                    Component.onCompleted: {
                        scrollSlider.rightTextWidth = width
                    }
                }

                tickDirection: D.TipsSlider.TickDirection.Back
                slider.handleType: Slider.HandleType.ArrowBottom
                slider.value: dccData.EraserPressureSensitive
                slider.from: 1
                slider.to: ticks.length
                slider.live: true
                slider.stepSize: 1
                slider.snapMode: Slider.SnapAlways
                ticks: [
                    D.SliderTipItem {
                        text: scrollSlider.tips[0]
                        highlight: scrollSlider.slider.value === 1
                    },
                    D.SliderTipItem {
                        text: scrollSlider.tips[1]
                        highlight: scrollSlider.slider.value === 2
                    },
                    D.SliderTipItem {
                        text: scrollSlider.tips[2]
                        highlight: scrollSlider.slider.value === 3
                    },
                    D.SliderTipItem {
                        text: scrollSlider.tips[3]
                        highlight: scrollSlider.slider.value === 4
                    },
                    D.SliderTipItem {
                        text: scrollSlider.tips[4]
                        highlight: scrollSlider.slider.value === 5
                    },
                    D.SliderTipItem {
                        text: scrollSlider.tips[5]
                        highlight: scrollSlider.slider.value === 6
                    },
                    D.SliderTipItem {
                        text: scrollSlider.tips[6]
                        highlight: scrollSlider.slider.value === 7
                    }
                ]
                slider.onValueChanged: {
                    if (dccData.EraserPressureSensitive !== slider.value)
                        dccData.EraserPressureSensitive = slider.value
                }
            }
        }
    }
}
