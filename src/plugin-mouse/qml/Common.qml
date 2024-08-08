// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc 1.0

Rectangle {
    id: frame
    // color: "red"
    height: childrenRect.height
    anchors {
        left: parent.left
        right: parent.right
        // leftMargin: 40 // 设置左边间距为20
        // rightMargin: 40
    }
    ColumnLayout{
        width: frame.width
        Text {
            font: D.DTK.fontManager.t4
            text: qsTr("Common")
            Layout.leftMargin: 10
        }
        D.BoxPanel {
            Layout.topMargin: 10
            Layout.preferredHeight: 140
            Layout.fillWidth: true
            backgroundFlowsHovered: D.ColorSelector.family === D.Palette.CrystalColor
            outsideBorderColor: null
            color2: color1
            ColumnLayout {
                width: parent.width
                Text {
                    id: speedText
                    Layout.topMargin: 10
                    font: D.DTK.fontManager.t7
                    text: qsTr("Scroll Speed")
                    Layout.leftMargin: 10
                }
                D.TipsSlider {
                    id: scrollSlider
                    readonly property var tips: [("1"), ("2"), ("3"), ("4"), ("5"), ("6"), ("7"), ("8"), ("9"), ("10")]
                    Layout.preferredHeight: 140
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
                    slider.snapMode:Slider.SnapAlways
                    ticks: [D.SliderTipItem {
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
                        }]
                    slider.onValueChanged: {
                        dccData.scrollSpeed = slider.value
                    }
                }
            }
        }
        D.BoxPanel {
            Layout.topMargin: 10
            Layout.preferredHeight: 140
            Layout.fillWidth: true
            backgroundFlowsHovered: D.ColorSelector.family === D.Palette.CrystalColor
            outsideBorderColor: null
            color2: color1
            RowLayout {
                width: parent.width
                ColumnLayout {
                    Text {
                        id: doubleClickText
                        Layout.topMargin: 10
                        font: D.DTK.fontManager.t7
                        text: qsTr("Double Click Speed")
                        Layout.leftMargin: 10
                    }
                    D.TipsSlider {
                        id: doubleClickSlider
                        readonly property var tips: [qsTr("Short"), (""), (""), (""), (""), (""), qsTr("Long")]
                        Layout.alignment: Qt.AlignCenter
                        Layout.margins: 10
                        Layout.fillWidth: true
                        tickDirection: D.TipsSlider.TickDirection.Back
                        slider.handleType: Slider.HandleType.ArrowBottom
                        slider.value: dccData.doubleSpeed
                        slider.from: 6
                        slider.to: 0
                        slider.live: true
                        slider.stepSize: 1
                        slider.snapMode:Slider.SnapAlways
                        ticks: [D.SliderTipItem {
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
                            }]
                        slider.onValueChanged: {
                            dccData.doubleSpeed = slider.value
                        }
                    }
                }
                ColumnLayout {
                    // Layout.preferredWidth: 200
                    Text {
                        id: scrollText
                        Layout.alignment: Qt.AlignLeft
                        font: D.DTK.fontManager.t7
                        text: qsTr("Double Click Test")
                    }
                    ClickTest {
                        id: doubleClickAni
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: 128
                        Layout.preferredHeight: 76
                    }

                }
            }
        }
        D.BoxPanel {
            Layout.topMargin: 10
            Layout.preferredHeight: 40
            Layout.fillWidth: true
            backgroundFlowsHovered: D.ColorSelector.family === D.Palette.CrystalColor
            outsideBorderColor: null
            color2: color1
            RowLayout {
                width: parent.width
                Text {
                    Layout.topMargin: 10
                    font: D.DTK.fontManager.t6
                    text: qsTr("Left Hand Mode")
                    Layout.leftMargin: 10
                    Layout.alignment: Qt.AlignLeft
                }
                D.Switch {
                    Layout.topMargin: 10
                    Layout.leftMargin: 10
                    Layout.alignment: Qt.AlignRight
                    checked: dccData.leftHandState
                    onCheckedChanged: {
                        dccData.leftHandState = checked
                    }
                }
            }
        }
    }
}
