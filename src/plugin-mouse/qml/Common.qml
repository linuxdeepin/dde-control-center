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
        parentName: "MouseAndTouchpadCommon"
        displayName: qsTr("General")
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
        name: "ScrollSpeed"
        parentName: "MouseAndTouchpadCommon"
        displayName: qsTr("Scrolling Speed")
        backgroundType: DccObject.Normal
        weight: 20
        pageType: DccObject.Item
        page: ColumnLayout {
            Layout.fillHeight: true
            Label {
                id: speedText
                Layout.topMargin: 10
                font: D.DTK.fontManager.t6
                text: dccObj.displayName
                Layout.leftMargin: 14
            }
            D.TipsSlider {
                id: scrollSlider
                readonly property var tips: [("1"), ("2"), ("3"), ("4"), ("5"), ("6"), ("7"), ("8"), ("9"), ("10")]
                Layout.preferredHeight: 90
                Layout.alignment: Qt.AlignCenter
                Layout.margins: 10
                Layout.fillWidth: true
                tickDirection: D.TipsSlider.TickDirection.Back
                slider.handleType: Slider.HandleType.ArrowBottom
                slider.value: dccData.scrollSpeed
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
                    },
                    D.SliderTipItem {
                        text: scrollSlider.tips[7]
                        highlight: scrollSlider.slider.value === 8
                    },
                    D.SliderTipItem {
                        text: scrollSlider.tips[8]
                        highlight: scrollSlider.slider.value === 9
                    },
                    D.SliderTipItem {
                        text: scrollSlider.tips[9]
                        highlight: scrollSlider.slider.value === 10
                    }
                ]
                slider.onValueChanged: {
                    dccData.scrollSpeed = slider.value
                }
            }
        }
    }
    DccObject {
        name: "DoubleClickSpeed"
        parentName: "MouseAndTouchpadCommon"
        displayName: qsTr("Double Click Speed")
        weight: 30
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
                        Layout.leftMargin: 14
                        font: D.DTK.fontManager.t6
                        text: dccObj.displayName
                    }
                    D.TipsSlider {
                        id: doubleClickSlider
                        readonly property var tips: [qsTr("Slow"), (""), (""), (""), (""), (""), qsTr("Fast")]
                        Layout.alignment: Qt.AlignCenter
                        Layout.margins: 10
                        Layout.fillWidth: true
                        tickDirection: D.TipsSlider.TickDirection.Back
                        slider.handleType: Slider.HandleType.ArrowBottom
                        slider.value: dccData.doubleSpeed
                        slider.from: 0
                        slider.to: 6
                        slider.live: true
                        slider.stepSize: 1
                        slider.snapMode: Slider.SnapAlways
                        ticks: [
                            D.SliderTipItem {
                                text: doubleClickSlider.tips[0]
                                // highlight: doubleClickSlider.slider.value === 1
                            },
                            D.SliderTipItem {
                                text: doubleClickSlider.tips[1]
                            },
                            D.SliderTipItem {
                                text: doubleClickSlider.tips[2]
                            },
                            D.SliderTipItem {
                                text: doubleClickSlider.tips[3]
                            },
                            D.SliderTipItem {
                                text: doubleClickSlider.tips[4]
                            },
                            D.SliderTipItem {
                                text: doubleClickSlider.tips[5]
                            },
                            D.SliderTipItem {
                                text: doubleClickSlider.tips[6]
                            }
                        ]
                        slider.onValueChanged: {
                            dccData.doubleSpeed = slider.value
                        }
                    }
                }
                ColumnLayout {
                    Layout.preferredWidth: 165
                    Label {
                        id: scrollText
                        Layout.alignment: Qt.AlignLeft
                        font: D.DTK.fontManager.t7
                        text: qsTr("Double Click Test")
                    }
                    ClickTest {
                        id: doubleClickAni
                        Layout.alignment: Qt.AlignHCenter
                        Layout.leftMargin: 8
                        Layout.preferredWidth: 150
                        Layout.preferredHeight: 76
                    }
                }
            }
        }
    }
    DccObject {
        name: "LeftHandMode"
        parentName: "MouseAndTouchpadCommon"
        displayName: qsTr("Left Hand Mode")
        weight: 40
        backgroundType: DccObject.Normal
        pageType: DccObject.Editor
        page: D.Switch {
            checked: dccData.leftHandState
            onCheckedChanged: {
                dccData.leftHandState = checked
            }
        }
    }
}
