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
    // height: childrenRect.height
    anchors {
        // left: parent.left
        // right: parent.right
        // leftMargin: 40 // 设置左边间距为20
        // rightMargin: 40
    }
    ColumnLayout{
        width: frame.width
        Text {
            font: D.DTK.fontManager.t4
            text: qsTr("Mouse")
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
                    text: qsTr("Pointer Speed")
                    Layout.leftMargin: 10
                }
                D.TipsSlider {
                    id: scrollSlider
                    readonly property var tips: [("Slow"), (""), (""), (""), (""), (""), ("Fast")]
                    Layout.preferredHeight: 140
                    Layout.alignment: Qt.AlignCenter
                    Layout.margins: 10
                    Layout.fillWidth: true
                    tickDirection: D.TipsSlider.TickDirection.Back
                    slider.handleType: Slider.HandleType.ArrowBottom
                    slider.value: dccData.mouseMoveSpeed
                    slider.from: 0
                    slider.to: ticks.length - 1
                    slider.live: true
                    slider.stepSize: 1
                    slider.snapMode:Slider.SnapAlways
                    ticks: [D.SliderTipItem {
                            text: scrollSlider.tips[0]
                            highlight: scrollSlider.slider.value === 0
                        },
                        D.SliderTipItem {
                            text: scrollSlider.tips[1]
                            highlight: scrollSlider.slider.value === 1
                        },
                        D.SliderTipItem {
                            text: scrollSlider.tips[2]
                            highlight: scrollSlider.slider.value === 2
                        },
                        D.SliderTipItem {
                            text: scrollSlider.tips[3]
                            highlight: scrollSlider.slider.value === 3
                        },
                        D.SliderTipItem {
                            text: scrollSlider.tips[4]
                            highlight: scrollSlider.slider.value === 4
                        },
                        D.SliderTipItem {
                            text: scrollSlider.tips[5]
                            highlight: scrollSlider.slider.value === 5
                        },
                        D.SliderTipItem {
                            text: scrollSlider.tips[6]
                            highlight: scrollSlider.slider.value === 6
                        }]
                    slider.onValueChanged: {
                        dccData.mouseMoveSpeed = slider.value
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
                        enabled: false
                        id: doubleClickText
                        Layout.topMargin: 10
                        font: D.DTK.fontManager.t7
                        text: qsTr("Pointer Size")
                        Layout.leftMargin: 10
                    }
                    D.TipsSlider {
                        id: doubleClickSlider
                        enabled: false
                        readonly property var tips: [qsTr("Short"), (""), (""), (""), (""), (""), qsTr("Long")]
                        Layout.alignment: Qt.AlignCenter
                        Layout.margins: 10
                        Layout.fillWidth: true
                        tickDirection: D.TipsSlider.TickDirection.Back
                        slider.handleType: Slider.HandleType.ArrowBottom
                        slider.value: dccData.scrollSpeed
                        slider.from: 200
                        slider.to: 800
                        slider.live: true
                        slider.stepSize: 100
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
            }
        }
        D.BoxPanel {
            Layout.topMargin: 10
            Layout.fillWidth: true
            Layout.preferredHeight: childrenRect.height
            backgroundFlowsHovered: D.ColorSelector.family === D.Palette.CrystalColor
            outsideBorderColor: null
            color2: color1
            ColumnLayout {
                width: parent.width
                RowLayout {
                    width: parent.width
                    Layout.preferredHeight: 40
                    Text {
                        font: D.DTK.fontManager.t6
                        text: qsTr("Mouse Acceleration")
                        Layout.leftMargin: 10
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    }
                    D.Switch {
                        Layout.rightMargin: 10
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        checked: dccData.accelProfile
                        onCheckedChanged: {
                            dccData.accelProfile = checked
                        }
                    }
                }
                RowLayout {
                    width: parent.width
                    Layout.preferredHeight: 40
                    visible: dccData.tpadExist
                    Text {
                        font: D.DTK.fontManager.t6
                        text: qsTr("Disable touchpad when a mouse is connected")
                        Layout.leftMargin: 10
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    }
                    D.Switch {
                        Layout.rightMargin: 10
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        checked: dccData.disTpad
                        onCheckedChanged: {
                            dccData.disTpad = checked
                        }
                    }
                }
                RowLayout {
                    width: parent.width
                    Layout.preferredHeight: 40
                    Text {
                        font: D.DTK.fontManager.t6
                        text: qsTr("Natural Scrolling")
                        Layout.leftMargin: 10
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    }
                    D.Switch {
                        Layout.rightMargin: 10
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        checked: dccData.mouseNaturalScroll
                        onCheckedChanged: {
                            dccData.mouseNaturalScroll = checked
                        }
                    }
                }
            }
        }
    }
}

