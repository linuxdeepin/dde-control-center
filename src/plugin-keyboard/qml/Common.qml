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
        name: "enableKeyboard"
        parentName: "KeyboardCommon"
        displayName: qsTr("Enable Keyboard")
        visible: false
        weight: 20
        backgroundType: DccObject.Normal
        pageType: DccObject.Editor
        page: D.Switch {
            checked: dccData.keyboardEnabled
            onCheckedChanged: {
                dccData.keyboardEnabled = checked
            }
        }
    }
    DccObject {
        name: "RepeatDelay"
        parentName: "KeyboardCommon"
        displayName: qsTr("Repeat delay")
        backgroundType: DccObject.Normal
        weight: 30
        visible: dccData.keyboardEnabled
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
        weight: 40
        visible: dccData.keyboardEnabled
        pageType: DccObject.Item
        page: DccGroupView {
            height: implicitHeight + 20
        }
    }

    DccObject {
        name: "RepeatRate"
        parentName: "RepeatRateGroup"
        displayName: qsTr("Repeat rate")
        weight: 50
        visible: dccData.keyboardEnabled
        backgroundType: DccObject.Normal
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
        weight: 60
        visible: dccData.keyboardEnabled
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: TextField {
            id: textField
            placeholderText: qsTr("test here")
            background: null
            horizontalAlignment: textMetrics.boundingRect.width > width ? Text.AlignRight : Text.AlignHCenter
            font: D.DTK.fontManager.t5
            onActiveFocusChanged: {
                if (!activeFocus) {
                    text = ""
                }
            }
            // 创建文本测量器
            TextMetrics {
                id: textMetrics
                font: textField.font  // 使用与 TextField 相同的字体
                text: textField.text  // 绑定到当前文本
            }
        }
    }

    DccObject {
        name: "KeypadSettings"
        parentName: "KeyboardCommon"
        weight: 70
        visible: dccData.keyboardEnabled
        pageType: DccObject.Item
        page: DccGroupView {
            height: implicitHeight + 20
        }
    }
    DccObject {
        name: "EnableNumLock"
        parentName: "KeypadSettings"
        displayName: qsTr("Numeric Keypad")
        weight: 80
        visible: dccData.keyboardEnabled
        backgroundType: DccObject.Normal
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
        weight: 90
        visible: dccData.keyboardEnabled
        backgroundType: DccObject.Normal
        pageType: DccObject.Editor
        page: D.Switch {
            checked: dccData.capsLock
            onCheckedChanged: {
                dccData.capsLock = checked
            }
        }
    }
}
