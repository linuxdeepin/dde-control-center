// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
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
        name: "Common"
        parentName: "KeyboardCommon"
        displayName: qsTr("Common")
        weight: 10
        hasBackground: false
        pageType: DccObject.Item
        page: ColumnLayout {
            Label {
                height: contentHeight
                Layout.leftMargin: 10
                font: D.DTK.fontManager.t4
                text: dccObj.displayName
            }
        }
    }
    DccObject {
        name: "RepeatDelay"
        parentName: "KeyboardCommon"
        displayName: qsTr("Repeat delay")
        hasBackground: true
        weight: 20
        pageType: DccObject.Item
        page: ColumnLayout {
            Layout.fillHeight: true
            Label {
                id: speedText
                Layout.topMargin: 10
                font: D.DTK.fontManager.t7
                text: dccObj.displayName
                Layout.leftMargin: 10
            }
            D.TipsSlider {
                id: scrollSlider
                readonly property var tips: [qsTr("Short"), (""), (""), (""), (""), (""), qsTr("Long")]
                Layout.preferredHeight: 90
                Layout.alignment: Qt.AlignCenter
                Layout.margins: 10
                Layout.fillWidth: true
                tickDirection: D.TipsSlider.TickDirection.Back
                slider.handleType: Slider.HandleType.ArrowBottom
                slider.value: dccData.repeatDelay
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
                    dccData.repeatDelay = slider.value
                }
            }
        }
    }
    DccObject {
        name: "RepeatRateGroup"
        parentName: "KeyboardCommon"
        weight: 30
        hasBackground: true
        pageType: DccObject.Item
        page: DccGroupView {
            spacing: 5
            height: implicitHeight + 20
        }
    }

    DccObject {
        name: "RepeatRate"
        parentName: "RepeatRateGroup"
        displayName: qsTr("Repeat rate")
        weight: 30
        hasBackground: true
        pageType: DccObject.Item
        page: Rectangle {
            color: "transparent"
            implicitHeight: rowView.height + 20
            RowLayout {
                id: rowView
                width: parent.width
                ColumnLayout {
                    Layout.fillHeight: true
                    Label {
                        id: doubleClickText
                        Layout.topMargin: 10
                        Layout.leftMargin: 10
                        font: D.DTK.fontManager.t7
                        text: dccObj.displayName
                    }
                    D.TipsSlider {
                        id: repeatRateSlider
                        readonly property var tips: [qsTr("Slow"), (""), (""), (""), (""), (""), qsTr("Fast")]
                        Layout.alignment: Qt.AlignCenter
                        Layout.margins: 10
                        Layout.fillWidth: true
                        tickDirection: D.TipsSlider.TickDirection.Back
                        slider.handleType: Slider.HandleType.ArrowBottom
                        slider.value: dccData.repeatInterval
                        slider.from: 1
                        slider.to: ticks.length
                        slider.live: true
                        slider.stepSize: 1
                        slider.snapMode: Slider.SnapAlways
                        ticks: [
                            D.SliderTipItem {
                                text: repeatRateSlider.tips[0]
                                highlight: repeatRateSlider.slider.value === 1
                            },
                            D.SliderTipItem {
                                text: repeatRateSlider.tips[1]
                                highlight: repeatRateSlider.slider.value === 2
                            },
                            D.SliderTipItem {
                                text: repeatRateSlider.tips[2]
                                highlight: repeatRateSlider.slider.value === 3
                            },
                            D.SliderTipItem {
                                text: repeatRateSlider.tips[3]
                                highlight: repeatRateSlider.slider.value === 4
                            },
                            D.SliderTipItem {
                                text: repeatRateSlider.tips[4]
                                highlight: repeatRateSlider.slider.value === 5
                            },
                            D.SliderTipItem {
                                text: repeatRateSlider.tips[5]
                                highlight: repeatRateSlider.slider.value === 6
                            },
                            D.SliderTipItem {
                                text: repeatRateSlider.tips[6]
                                highlight: repeatRateSlider.slider.value === 7
                            }
                        ]
                        slider.onValueChanged: {
                            dccData.repeatInterval = slider.value
                        }
                    }
                }
            }
        }
    }

    DccObject {
        name: "EditTesting"
        parentName: "RepeatRateGroup"
        displayName: qsTr("Numeric Keypad")
        weight: 40
        hasBackground: true
        pageType: DccObject.Item
        page: TextField {
            placeholderText: qsTr("test here")
            background: null
            horizontalAlignment: Text.AlignHCenter
            font: D.DTK.fontManager.t5
        }
    }

    DccObject {
        name: "KeypadSettings"
        parentName: "KeyboardCommon"
        displayName: qsTr("Numeric Keypad")
        weight: 40
        hasBackground: true
        pageType: DccObject.Item
        page: DccGroupView {
            spacing: 5
            height: implicitHeight + 20
        }
    }
    DccObject {
        name: "EnableNumLock"
        parentName: "KeypadSettings"
        displayName: qsTr("Numeric Keypad")
        weight: 40
        hasBackground: true
        pageType: DccObject.Editor
        page: D.Switch {
            checked: dccData.numLock
            onCheckedChanged: {
                dccData.numLock = checked
            }
        }
    }
    DccObject {
        name: "CapsLockPrompt"
        parentName: "KeypadSettings"
        displayName: qsTr("Caps lock prompt")
        weight: 50
        hasBackground: true
        pageType: DccObject.Editor
        page: D.Switch {
            checked: dccData.capsLock
            onCheckedChanged: {
                dccData.capsLock = checked
            }
        }
    }
}
