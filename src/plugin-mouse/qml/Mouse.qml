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
        name: "Mouse"
        parentName: "MouseAndTouchpadMouse"
        displayName: qsTr("Mouse")
        weight: 10
        pageType: DccObject.Item
        page: ColumnLayout {
            Label {
                height: contentHeight
                // implicitHeight: 200
                Layout.leftMargin: 10
                font: D.DTK.fontManager.t4
                text: dccObj.displayName
            }
        }
    }
    DccObject {
        name: "PointerSpeed"
        parentName: "MouseAndTouchpadMouse"
        displayName: qsTr("Pointer Speed")
        weight: 20
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: ColumnLayout {
            Label {
                id: speedText
                Layout.topMargin: 10
                font: D.DTK.fontManager.t6
                text: dccObj.displayName
                Layout.leftMargin: 14
            }
            D.TipsSlider {
                id: scrollSlider
                readonly property var tips: [qsTr("Slow"), (""), (""), (""), (""), (""), qsTr("Fast")]
                Layout.preferredHeight: 100
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
                slider.snapMode: Slider.SnapAlways
                ticks: [
                    D.SliderTipItem {
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
                    }
                ]
                slider.onValueChanged: {
                    dccData.mouseMoveSpeed = slider.value
                }
            }
        }
    }
    DccObject {
        name: "PointerSize"
        parentName: "MouseAndTouchpadMouse"
        displayName: qsTr("Pointer Size")
        weight: 30
        visible: false
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: RowLayout {
            ColumnLayout {
                Label {
                    enabled: false
                    id: doubleClickText
                    Layout.topMargin: 10
                    font: D.DTK.fontManager.t6
                    text: dccObj.displayName
                    Layout.leftMargin: 14
                }
                D.TipsSlider {
                    id: doubleClickSlider
                    enabled: false
                    readonly property var tips: [qsTr("Short"), (""), (""), (""), (""), (""), qsTr("Long")]
                    Layout.preferredHeight: 100
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
        }
    }
    DccObject {
        name: "MouseSettings"
        parentName: "MouseAndTouchpadMouse"
        weight: 30
        pageType: DccObject.Item
        page: DccGroupView {}
        DccObject {
            name: "MouseAcceleration"
            parentName: "MouseSettings"
            displayName: qsTr("Mouse Acceleration")
            weight: 10
            pageType: DccObject.Editor
            page: D.Switch {
                checked: dccData.accelProfile
                onCheckedChanged: {
                    dccData.accelProfile = checked
                }
            }
        }
        DccObject {
            name: "DisableTouchpad"
            parentName: "MouseSettings"
            displayName: qsTr("Disable touchpad when a mouse is connected")
            weight: 20
            visible: dccData.tpadExist
            pageType: DccObject.Editor
            page: D.Switch {
                checked: dccData.disTpad
                onCheckedChanged: {
                    dccData.disTpad = checked
                }
            }
        }
        DccObject {
            name: "NaturalScrolling"
            parentName: "MouseSettings"
            displayName: qsTr("Natural Scrolling")
            weight: 30
            pageType: DccObject.Editor
            page: D.Switch {
                checked: dccData.mouseNaturalScroll
                onCheckedChanged: {
                    dccData.mouseNaturalScroll = checked
                }
            }
        }
    }
}
