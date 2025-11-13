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
    property bool isHovering: false
    property bool animatedImagePlaying: false
    property var preActionDec: undefined
    // Animation priority control: ensure selection animation has priority over hover animation
    property bool isPlayingSelectionAnimation: false
    property int animationPriority: 0  // 0: no animation, 1: hover animation, 2: selection animation
    // Record current hover state details for switching after selection animation ends
    property int currentHoverIndex: -1
    property var currentHoverActionDec: undefined
    property int currentFingerNum: 3  // Record current gesture finger count
    property bool resetAnimationTrigger: false  // Trigger to reset animation frames
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
        visible: dccData.touchpadSwitchFileExists
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
                Layout.leftMargin: 14
                font: D.DTK.fontManager.t6
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
                        id: gestureFingerAnimatedImage
                        source: dccData.gestureFingerAniPath
                        Layout.alignment: Qt.AlignCenter
                        sourceSize.width: 204
                        sourceSize.height: 134
                        playing: touchpad.animatedImagePlaying

                        // Reset animation frame when new selection is made
                        Connections {
                            target: touchpad
                            function onResetAnimationTriggerChanged() {
                                gestureFingerAnimatedImage.currentFrame = 0
                            }
                        }

                        onFrameChanged: {
                            if (currentFrame >= (frameCount - 1)) {
                                currentFrame = 0
                                if (touchpad.animationPriority === 2) {
                                    // Selection animation completed, reset priority
                                    touchpad.isPlayingSelectionAnimation = false
                                    touchpad.animationPriority = 0

                                    // If currently hovering, immediately play hover animation
                                    if (touchpad.isHovering && touchpad.currentHoverIndex >= 0 && touchpad.currentHoverActionDec) {
                                        // Update animation path to the hovered action
                                        dccData.updateFigerGestureAni(touchpad.currentFingerNum, touchpad.currentHoverIndex, touchpad.currentHoverActionDec)
                                        touchpad.animationPriority = 1
                                        touchpad.animatedImagePlaying = true
                                        return // Continue playing hover animation
                                    }
                                }
                                if (!touchpad.isHovering || touchpad.animationPriority === 2) {
                                    touchpad.animatedImagePlaying = false;
                                }
                            }
                        }
                    }

                    AnimatedImage {
                        id: gestureActionAnimatedImage
                        source: dccData.gestureActionAniPath
                        Layout.alignment: Qt.AlignCenter
                        sourceSize.width: 204
                        sourceSize.height: 134
                        playing:  touchpad.animatedImagePlaying

                        // Reset animation frame when new selection is made
                        Connections {
                            target: touchpad
                            function onResetAnimationTriggerChanged() {
                                gestureActionAnimatedImage.currentFrame = 0
                            }
                        }

                        onFrameChanged: {
                            if (currentFrame >= (frameCount - 1)) {
                                currentFrame = 0
                                if (touchpad.animationPriority === 2) {
                                    // Selection animation completed, reset priority
                                    touchpad.isPlayingSelectionAnimation = false
                                    touchpad.animationPriority = 0

                                    // If currently hovering, immediately play hover animation
                                    if (touchpad.isHovering && touchpad.currentHoverIndex >= 0 && touchpad.currentHoverActionDec) {
                                        // Update animation path to the hovered action
                                        dccData.updateFigerGestureAni(touchpad.currentFingerNum, touchpad.currentHoverIndex, touchpad.currentHoverActionDec)
                                        touchpad.animationPriority = 1
                                        touchpad.animatedImagePlaying = true
                                        return // Continue playing hover animation
                                    }
                                }
                                if (!touchpad.isHovering || touchpad.animationPriority === 2) {
                                    touchpad.animatedImagePlaying = false;
                                }
                            }
                        }
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
                dccData.updateFigerGestureAni(3, index, actionDec)

                // Reset animation frames to ensure full playback from the beginning
                touchpad.resetAnimationTrigger = !touchpad.resetAnimationTrigger

                // Set high priority for selection animation
                touchpad.animationPriority = 2
                touchpad.isPlayingSelectionAnimation = true
                touchpad.animatedImagePlaying = true
            }

            onHoveredChanged: function (index, actionDec, hovered) {
                if (hovered) {
                    // Record hover state details
                    touchpad.currentHoverIndex = index
                    touchpad.currentHoverActionDec = actionDec
                    touchpad.currentFingerNum = 3  // Three-finger gesture

                    // Only play hover animation when no selection animation is playing
                    if (!touchpad.isPlayingSelectionAnimation && touchpad.animationPriority < 2) {
                        dccData.updateFigerGestureAni(3, index, actionDec)
                        preActionDec = actionDec
                        touchpad.isHovering = true
                        touchpad.animationPriority = 1
                        touchpad.animatedImagePlaying = true
                    } else {
                        // Record hover state but don't play animation
                        preActionDec = actionDec
                        touchpad.isHovering = true
                    }
                } else if (preActionDec === actionDec) {
                    touchpad.isHovering = false
                    preActionDec = undefined
                    touchpad.currentHoverIndex = -1
                    touchpad.currentHoverActionDec = undefined
                    if (touchpad.animationPriority === 1) {
                        touchpad.animationPriority = 0
                    }
                }
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
                dccData.updateFigerGestureAni(4, index, actionDec)

                // Reset animation frames to ensure full playback from the beginning
                touchpad.resetAnimationTrigger = !touchpad.resetAnimationTrigger

                // Set high priority for selection animation
                touchpad.animationPriority = 2
                touchpad.isPlayingSelectionAnimation = true
                touchpad.animatedImagePlaying = true
            }

            onHoveredChanged: function (index, actionDec, hovered) {
                if (hovered) {
                    // Record hover state details (four-finger gesture)
                    touchpad.currentHoverIndex = index
                    touchpad.currentHoverActionDec = actionDec
                    touchpad.currentFingerNum = 4  // Four-finger gesture

                    // Only play hover animation when no selection animation is playing
                    if (!touchpad.isPlayingSelectionAnimation && touchpad.animationPriority < 2) {
                        dccData.updateFigerGestureAni(4, index, actionDec)
                        preActionDec = actionDec
                        touchpad.isHovering = true
                        touchpad.animationPriority = 1
                        touchpad.animatedImagePlaying = true
                    } else {
                        // Record hover state but don't play animation
                        preActionDec = actionDec
                        touchpad.isHovering = true
                    }
                } else if (preActionDec === actionDec) {
                    touchpad.isHovering = false
                    preActionDec = undefined
                    touchpad.currentHoverIndex = -1
                    touchpad.currentHoverActionDec = undefined
                    if (touchpad.animationPriority === 1) {
                        touchpad.animationPriority = 0
                    }
                }
            }
        }

        onParentItemChanged: item => {
            if (item) {
                item.topPadding = 3
            }
        }
    }

}
