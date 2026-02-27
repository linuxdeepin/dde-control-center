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
        pageType: DccObject.Item
        backgroundType: DccObject.Normal
        page: ColumnLayout {
            anchors.fill: parent
            Label {
                Layout.topMargin: 10
                text: dccObj.displayName
                Layout.leftMargin: 14
            }

            Flow {
                id: listview
                Layout.fillWidth: true
                Layout.leftMargin: 10
                opacity: enabled ? 1 : 0.4
                property var tips: [qsTr("Small"), qsTr("Medium"), qsTr("Large")]
                property var icons: ["mouse_cursor_size_small.png", "mouse_cursor_size_medium.png", "mouse_cursor_size_big.png"]
                property var availables: [false, false, false]
                property var availableSizes: [24, 36, 48]
                property var availableSizesModel: dccData.availableCursorSizes

                onAvailableSizesModelChanged: {
                    availables = [true, false, false]
                    for (let i = 0; i < availableSizesModel.length; i++) {
                        let size = availableSizesModel[i]
                        if (size > 30 && !availables[1]) {
                            availables[1] = true
                        } else if (size > 42 && !availables[2]) {
                            availables[2] = true
                        }
                    }
                }

                spacing: 8
                Repeater {
                    model: listview.tips.length
                    ColumnLayout {
                        id: layout
                        visible: listview.availables[index]
                        property bool checked: {
                            let cutCursorSize = dccData.cursorSize
                            if (index === 0 && cutCursorSize > 0 && cutCursorSize <= 30) {
                                return true
                            } else if (index === 1 && cutCursorSize > 30 && cutCursorSize <= 42) {
                                return true
                            } else if (index === 2 && cutCursorSize > 42) {
                                return true
                            }
                            return false
                        }
                        width: 112
                        height: 104
                        Item {
                            Layout.preferredHeight: 78
                            Layout.fillWidth: true

                            Rectangle {
                                anchors.fill: parent
                                color: "transparent"
                                border.width: 2
                                border.color: layout.checked ? D.DTK.platformTheme.activeColor : "transparent"
                                radius: 7
                                Control {
                                    id: iconControl
                                    anchors.fill: parent
                                    anchors.margins: 4
                                    
                                    contentItem: Image {
                                        sourceSize: Qt.size(width, height)
                                        source: "qrc:/icons/deepin/builtin/icons/" + (D.DTK.themeType === D.ApplicationHelper.LightType ? "light/" : "dark/") + listview.icons[index]
                                    }
                                }
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (dccData.cursorSize === listview.availableSizes[index]) {
                                        return
                                    }
                                    dccData.cursorSize = listview.availableSizes[index]
                                    pointerSizeTips.visible = true
                                }
                            }
                        }

                        Text {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            text: listview.tips[index]
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            color: layout.checked ? D.DTK.platformTheme.activeColor : this.palette.windowText
                        }
                    }
                }
            }

            Item {
                id: pointerSizeTips
                visible: false
                Layout.fillWidth: true
                Layout.preferredHeight: tipsRow.implicitHeight
                RowLayout {
                    id: tipsRow
                    anchors.horizontalCenter: parent.horizontalCenter
                    D.DciIcon {
                        name: "tip_warning"
                        sourceSize: Qt.size(18, 18)
                        Layout.leftMargin: 10
                    }

                    Label {
                        Layout.fillWidth: true
                        font: D.DTK.fontManager.t7
                        text: qsTr("Some apps require logout or system restart to take effect")
                        wrapMode: Text.Wrap
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 6
            }
        }
    }

    DccObject {
        name: "MouseSettings"
        parentName: "MouseAndTouchpadMouse"
        weight: 50
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
