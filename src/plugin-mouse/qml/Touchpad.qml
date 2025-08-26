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
    id: touchpad
    property bool enabled : dccData.tapEnabled
    DccObject {
        name: "BasicSettings"
        parentName: "MouseAndTouchpad/Touchpad"
        displayName: qsTr("Basic Settings")
        weight: 10
        pageType: DccObject.Item

        page: ColumnLayout {
            Label {
                Layout.leftMargin: 10
                font.pixelSize: D.DTK.fontManager.t5.pixelSize
                font.weight: 500
                color: D.DTK.themeType === D.ApplicationHelper.LightType ?
                    Qt.rgba(0, 0, 0, 1) : Qt.rgba(1, 1, 1, 1)
                text: dccObj.displayName
            }
        }
        onParentItemChanged: item => {
            if (item) {
                item.bottomPadding = 4
            }
        }
    }

    DccObject {
        name: "DisableTouchpadByMouse"
        parentName: "MouseAndTouchpad/Touchpad"
        displayName: qsTr("Touchpad")
        weight: 20
        backgroundType: DccObject.Normal
        pageType: DccObject.Editor
        page: D.Switch {
            Layout.rightMargin: 10
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            checked: dccData.tapEnabled
            onCheckedChanged: {
                if (checked !== dccData.tapEnabled) {
                    dccData.tapEnabled = checked;
                }
            }
        }
        onParentItemChanged: item => {
            if (item) {
                item.topInset = 4
                item.bottomInset = 3
            }
        }
    }

    DccObject {
        name: "PointerSpeed"
        parentName: "MouseAndTouchpad/Touchpad"
        displayName: qsTr("Pointer Speed")
        weight: 30
        visible: touchpad.enabled
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
        onParentItemChanged: item => {
            if (item) {
                item.topPadding = 0
                item.bottomPadding = 3
            }
        }
    }

    DccObject {
        name: "TouchpadGroup"
        parentName: "MouseAndTouchpad/Touchpad"
        displayName: qsTr("Pointer Speed")
        weight: 40
        visible: touchpad.enabled
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
                checked: dccData.disIfTyping
                onCheckedChanged: {
                    if (checked !== dccData.disIfTyping) {
                        dccData.disIfTyping = checked;
                    }
                }
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
                    if (checked !== dccData.tapClick) {
                        dccData.tapClick = checked;
                    }
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
                    if (checked !== dccData.tpadNaturalScroll) {
                        dccData.tpadNaturalScroll = checked;
                    }
                }
            }

        }

        page: DccGroupView {
        }

        onParentItemChanged: item => {
            if (item) {
                item.topPadding = 3
                item.bottomPadding = 8
            }
        }
    }

    DccObject {
        name: "Gesture"
        parentName: "MouseAndTouchpad/Touchpad"
        displayName: qsTr("Gestures")
        weight: 50
        visible: touchpad.enabled
        pageType: DccObject.Item

        page: ColumnLayout {
            Label {
                Layout.leftMargin: 10
                font.pixelSize: D.DTK.fontManager.t5.pixelSize
                font.weight: 500
                color: D.DTK.themeType === D.ApplicationHelper.LightType ?
                    Qt.rgba(0, 0, 0, 1) : Qt.rgba(1, 1, 1, 1)
                text: dccObj.displayName
            }
        }

        onParentItemChanged: item => {
            if (item) {
                item.topPadding = 8
                item.bottomPadding = 4
            }
        }
    }

    DccObject {
        name: "GestureGroup"
        parentName: "MouseAndTouchpad/Touchpad"
        weight: 60
        visible: touchpad.enabled
        pageType: DccObject.Item

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

                    AnimatedImage {
                        source: dccData.gestureFingerAniPath
                        Layout.alignment: Qt.AlignCenter
                        sourceSize.width: 204
                        sourceSize.height: 134
                    }

                    AnimatedImage {
                        source: dccData.gestureActionAniPath
                        Layout.alignment: Qt.AlignCenter
                        sourceSize.width: 204
                        sourceSize.height: 134
                    }

                }

            }

        }

        page: DccGroupView {
        }

        onParentItemChanged: item => {
            if (item) {
                item.topPadding = 4
                item.bottomPadding = 6
            }
        }
    }

    DccObject {
        name: "ThreeFingerGesture"
        parentName: "MouseAndTouchpad/Touchpad"
        displayName: qsTr("Three-finger gestures")
        weight: 70
        visible: touchpad.enabled
        backgroundType: DccObject.AutoBg
        pageType: DccObject.Item

        page: ColumnLayout {
            Label {
                Layout.leftMargin: 10
                font: D.DTK.fontManager.t6
                text: dccObj.displayName
            }

        }

        onParentItemChanged: item => {
            if (item) {
                item.topPadding = 6
                item.bottomPadding = 3
            }
        }
    }

    DccObject {
        name: "ThreeFingerGestureGroup"
        parentName: "MouseAndTouchpad/Touchpad"
        weight: 80
        visible: touchpad.enabled
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: GestureGroup {
            model: dccData.threeFingerGestureModel()

            onComboIndexChanged: function (index, actionDec){
                dccData.setGestures(3, index, actionDec)
                dccData.updateFigerGestureAni(3,index, actionDec)
            }

            onHoveredChanged: function (index, actionDec) {
                dccData.updateFigerGestureAni(3,index, actionDec)
            }

        }
        onParentItemChanged: item => {
            if (item) {
                item.topPadding = 3
                item.bottomPadding = 6
            }
        }
    }

    DccObject {
        name: "FourFingerGesture"
        parentName: "MouseAndTouchpad/Touchpad"
        displayName: qsTr("Four-finger gestures")
        weight: 90
        visible: touchpad.enabled
        backgroundType: DccObject.AutoBg
        pageType: DccObject.Item
        page: ColumnLayout {
            Label {
                Layout.leftMargin: 10
                font: D.DTK.fontManager.t6
                text: dccObj.displayName
            }

        }
        onParentItemChanged: item => {
            if (item) {
                item.topPadding = 6
                item.bottomPadding = 3
            }
        }
    }

    DccObject {
        name: "FourFingerGestureGroup"
        parentName: "MouseAndTouchpad/Touchpad"
        weight: 100
        visible: touchpad.enabled
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: GestureGroup {
            model: dccData.fourFigerGestureModel()
            onComboIndexChanged: function (index, actionDec){
                dccData.setGestures(4, index, actionDec)
            }

            onHoveredChanged: function (index, actionDec) {
                dccData.updateFigerGestureAni(4,index, actionDec)
            }
        }

        onParentItemChanged: item => {
            if (item) {
                item.topPadding = 3
            }
        }
    }

}
