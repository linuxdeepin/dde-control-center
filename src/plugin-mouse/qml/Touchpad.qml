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
        name: "BasicSettings"
        parentName: "MouseAndTouchpad/Touchpad"
        displayName: qsTr("Basic Settings")
        weight: 10
        pageType: DccObject.Item

        page: ColumnLayout {
            Label {
                Layout.leftMargin: 10
                font: D.DTK.fontManager.t4
                text: dccObj.displayName
            }

        }

    }

    DccObject {
        name: "DisableTouchpadByMouse"
        parentName: "MouseAndTouchpad/Touchpad"
        displayName: qsTr("Disable touchpad when a mouse is connected")
        weight: 20
        backgroundType: DccObject.Normal
        pageType: DccObject.Editor
        page: D.Switch {
            Layout.rightMargin: 10
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            checked: dccData.disTpad
            onCheckedChanged: {
                dccData.disTpad = checked;
            }
        }

    }

    DccObject {
        name: "PointerSpeed"
        parentName: "MouseAndTouchpad/Touchpad"
        displayName: qsTr("Pointer Speed")
        weight: 30
        backgroundType: DccObject.Normal
        pageType: DccObject.Item

        page: ColumnLayout {
            Label {
                id: speedText

                Layout.topMargin: 10
                Layout.leftMargin: 10
                font: D.DTK.fontManager.t7
                text: dccObj.displayName
            }

            D.TipsSlider {
                id: scrollSlider

                readonly property var tips: [qsTr("Slow"), (""), (""), (""), (""), (""), qsTr("Fast")]

                Layout.preferredHeight: 90
                Layout.alignment: Qt.AlignCenter
                Layout.margins: 10
                Layout.fillWidth: true
                tickDirection: D.TipsSlider.TickDirection.Back
                slider.handleType: Slider.HandleType.ArrowBottom
                slider.value: dccData.tpadMoveSpeed
                slider.from: 0
                slider.to: ticks.length - 1
                slider.live: true
                slider.stepSize: 1
                slider.snapMode: Slider.SnapAlways
                slider.onValueChanged: {
                    dccData.tpadMoveSpeed = slider.value;
                }
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
            }

        }

    }

    DccObject {
        name: "TouchpadGroup"
        parentName: "MouseAndTouchpad/Touchpad"
        displayName: qsTr("Pointer Speed")
        weight: 40
        pageType: DccObject.Item

        DccObject {
            name: "DisableTouchpadByInput"
            parentName: "MouseAndTouchpad/Touchpad/TouchpadGroup"
            displayName: qsTr("Disable touchpad during input")
            weight: 10
            pageType: DccObject.Editor

            page: D.Switch {
                Layout.rightMargin: 10
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            }

        }

        DccObject {
            name: "TapToClick"
            parentName: "MouseAndTouchpad/Touchpad/TouchpadGroup"
            displayName: qsTr("Tap to Click")
            weight: 20
            pageType: DccObject.Editor

            page: D.Switch {
                Layout.rightMargin: 10
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                checked: dccData.tapClick
                onCheckedChanged: {
                    dccData.tapClick = checked;
                }
            }

        }

        DccObject {
            name: "NaturalScrolling"
            parentName: "MouseAndTouchpad/Touchpad/TouchpadGroup"
            displayName: qsTr("Natural Scrolling")
            weight: 30
            pageType: DccObject.Editor

            page: D.Switch {
                Layout.rightMargin: 10
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                checked: dccData.tpadNaturalScroll
                onCheckedChanged: {
                    dccData.tpadNaturalScroll = checked;
                }
            }

        }

        page: DccGroupView {
        }

    }

    DccObject {
        name: "Gesture"
        parentName: "MouseAndTouchpad/Touchpad"
        displayName: qsTr("Gesture")
        weight: 50
        pageType: DccObject.Item
        visible: false

        page: ColumnLayout {
            Label {
                Layout.leftMargin: 10
                font: D.DTK.fontManager.t4
                text: dccObj.displayName
            }

        }

    }

    DccObject {
        name: "GestureGroup"
        parentName: "MouseAndTouchpad/Touchpad"
        weight: 60
        pageType: DccObject.Item
        visible: false

        DccObject {
            name: "animation"
            parentName: "MouseAndTouchpad/Touchpad/GestureGroup"
            weight: 10
            pageType: DccObject.Item

            page: Rectangle {
                color: "transparent"
                implicitHeight: rowView.height + 20

                RowLayout {
                    id: rowView

                    width: parent.width
                    anchors.centerIn: parent

                    Image {
                        source: "qrc:/icons/deepin/builtin/icons/dcc_nav_touchpad_1_42px.png"
                        // fillMode: Image.PreserveAspectFit
                        Layout.alignment: Qt.AlignCenter
                        sourceSize.width: 204
                        sourceSize.height: 134
                    }

                    Image {
                        source: "qrc:/icons/deepin/builtin/icons/dcc_nav_touchpad_2_42px.png"
                        Layout.alignment: Qt.AlignCenter
                        sourceSize.width: 204
                        sourceSize.height: 134
                    }

                }

            }

        }

        page: DccGroupView {
        }

    }

    DccObject {
        name: "ThreeFingerGesture"
        parentName: "MouseAndTouchpad/Touchpad"
        displayName: qsTr("Three-finger gestures")
        weight: 70
        backgroundType: DccObject.AutoBg
        pageType: DccObject.Item
        visible: false

        page: ColumnLayout {
            Label {
                Layout.leftMargin: 10
                font: D.DTK.fontManager.t4
                text: dccObj.displayName
            }

        }

    }

    DccObject {
        name: "ThreeFingerGestureGroup"
        parentName: "MouseAndTouchpad/Touchpad"
        weight: 80
        backgroundType: DccObject.AutoBg
        //hasBackground: false
        visible: false
        pageType: DccObject.Item

        DccObject {
            name: "ThreeFingersUp"
            parentName: "MouseAndTouchpad/Touchpad/ThreeFingerGestureGroup"
            displayName: qsTr("Three-finger up")
            icon: "Two_fingers_up_and_down"
            weight: 20
            pageType: DccObject.Editor

            page: D.ComboBox {
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                model: [qsTr("Scroll Page")]
                currentIndex: 0
            }

        }

        DccObject {
            name: "ThreeFingersDown"
            parentName: "MouseAndTouchpad/Touchpad/ThreeFingerGestureGroup"
            displayName: qsTr("Three-finger down")
            icon: "Two_fingers_up_and_down"
            weight: 30
            pageType: DccObject.Editor

            page: D.ComboBox {
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                model: [qsTr("Scroll Page")]
                currentIndex: 0
            }

        }

        DccObject {
            name: "ThreeFingersLeft"
            parentName: "MouseAndTouchpad/Touchpad/ThreeFingerGestureGroup"
            displayName: qsTr("Three-finger left")
            icon: "Two_fingers_up_and_down"
            weight: 40
            pageType: DccObject.Editor

            page: D.ComboBox {
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                model: [qsTr("Scroll Page")]
                currentIndex: 0
            }

        }

        DccObject {
            name: "ThreeFingersRight"
            parentName: "MouseAndTouchpad/Touchpad/ThreeFingerGestureGroup"
            displayName: qsTr("Three-finger right")
            icon: "Two_fingers_up_and_down"
            weight: 50
            pageType: DccObject.Editor

            page: D.ComboBox {
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                model: [qsTr("Scroll Page")]
                currentIndex: 0
            }

        }

        page: DccGroupView {
        }

    }

    DccObject {
        name: "FourFingerGesture"
        parentName: "MouseAndTouchpad/Touchpad"
        displayName: qsTr("Four-finger gestures")
        weight: 90
        backgroundType: DccObject.AutoBg
        pageType: DccObject.Item
        visible: false
        page: ColumnLayout {
            Label {
                Layout.leftMargin: 10
                font: D.DTK.fontManager.t4
                text: dccObj.displayName
            }

        }

    }

    DccObject {
        name: "FourFingerGestureGroup"
        parentName: "MouseAndTouchpad/Touchpad"
        weight: 100
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        visible: false
        page: GestureGroup {

            model: ListModel {
                ListElement {
                    name: qsTr("Four-finger up")
                    iconName: "Two_fingers_up_and_down"
                }

                ListElement {
                    name: qsTr("Four-finger down")
                    iconName: "Two_fingers_up_and_down"
                }

                ListElement {
                    name: qsTr("Four-finger left")
                    iconName: "Two_fingers_up_and_down"
                }

                ListElement {
                    name: qsTr("Four-finger right")
                    iconName: "Two_fingers_up_and_down"
                }

                ListElement {
                    name: qsTr("Four-finger tap")
                    iconName: "Two_fingers_up_and_down"
                }

            }

        }

    }

}
