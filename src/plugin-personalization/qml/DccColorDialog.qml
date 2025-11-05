// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Dialogs
import QtQuick.Dialogs.quickimpl
import QtQuick.Layouts
import QtQuick.Templates as T

import org.deepin.dtk as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc.personalization 1.0

ColorDialogImpl {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding,
                            implicitHeaderWidth,
                            implicitFooterWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding
                             + (implicitHeaderHeight > 0 ? implicitHeaderHeight + spacing : 0)
                             + (implicitFooterHeight > 0 ? implicitFooterHeight + spacing : 0))

    topPadding: 6
    leftPadding: 6
    rightPadding: 6

    background: Loader {
        sourceComponent: D.FloatingPanel {
            radius: DS.Style.floatingPanel.radius
            blurMultiplier: 8.0
            backgroundColor: DS.Style.floatingPanel.background
            backgroundNoBlurColor: DS.Style.floatingPanel.backgroundNoBlur
        }
    }

    // ensure set sucessfully, default is 0
    Component.onCompleted: {
        alpha = 1.0
    }

    Connections {
        target: dccData
        function onColorPicked(color) {
            control.color = color
        }
        function onPickerError(error) {
            console.warn("deepin-picker error:", error)
        }
    }

    // Use deepin-picker if available, otherwise fallback to Qt's built-in eyeDropper
    ColorDialogImpl.eyeDropperButton: dccData.pickerAvailable ? null : eyeDropperButton
    ColorDialogImpl.colorPicker: colorPicker

    contentItem: ColumnLayout {
        DccSaturationLightnessPicker {
            id: colorPicker
            color: control.color
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 20
            Layout.topMargin: 2
            Layout.bottomMargin: 2
            Rectangle {
                Layout.preferredHeight: 20
                Layout.preferredWidth: 20
                color: control.color
                radius: 3
            }

            Slider {
                id: hueSlider
                orientation: Qt.Horizontal
                value: control.hue
                implicitHeight: 20
                onMoved: function() { control.hue = value; }
                handle: Rectangle {
                    implicitWidth: 6
                    height: hueSlider.height
                    border.color: Qt.rgba(0, 0, 0, 0.2)
                    x: hueSlider.leftPadding + (hueSlider.horizontal
                                                ? hueSlider.visualPosition * (hueSlider.availableWidth - width)
                                                : (hueSlider.availableWidth - width) / 2)
                    y: hueSlider.topPadding + (hueSlider.horizontal
                                               ? (hueSlider.availableHeight - height) / 2
                                               : hueSlider.visualPosition * (hueSlider.availableHeight - height))
                }
                background: Item {
                    anchors.fill: parent
                    anchors.leftMargin: hueSlider.handle.width / 2
                    anchors.rightMargin: hueSlider.handle.width / 2
                    Rectangle {
                        anchors.fill: parent
                        anchors.topMargin: 4
                        anchors.bottomMargin: 4
                        radius: 3
                        gradient: HueGradient {
                            orientation: Gradient.Horizontal
                        }
                    }
                }

                Layout.fillWidth: true
            }
        }

        GridLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 24
            Layout.topMargin: 2
            Layout.bottomMargin: 2
            columns: 5
            rows: 2
            D.IconButton {
                id: eyeDropperButton
                icon.name: "color_extractor"
                icon.width: 24
                textColor: D.Palette {
                    normal: Qt.rgba(0, 0, 0, 1)
                    normalDark: Qt.rgba(1, 1, 1, 1)
                }
                flat: true
                visible: true
                Layout.preferredWidth: implicitHeight

                background: Rectangle {
                    implicitWidth: 24
                    implicitHeight: 24
                    radius: DS.Style.control.radius
                    visible: eyeDropperButton.hovered
                    color: eyeDropperButton.palette.windowText
                    opacity: 0.1
                }

                    onClicked: {
                        // Use deepin-picker if available, Qt's eyeDropper will handle it otherwise
                        if (dccData.pickerAvailable) {
                            dccData.startPicker()
                        }
                    }
            }

            D.TextField {
                text: control.color.toString().substring(1).toUpperCase()
                Layout.preferredWidth: 70
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                validator: RegularExpressionValidator {
                    regularExpression: /^[0-9a-fA-F]{6}$/
                }
                onEditingFinished: {
                    control.color = "#" + text.toUpperCase()
                }
            }

            Repeater {
                model: [control.red, control.green, control.blue]
                D.TextField {
                    text: modelData
                    Layout.fillWidth: true
                    Layout.preferredWidth: 70
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    inputMethodHints: Qt.ImhDigitsOnly
                    validator: IntValidator { bottom: 0; top: 255 }

                    onEditingFinished: {
                        if (text !== "") {
                            if (modelData === control.red) {
                                control.red = text;
                            } else if (modelData === control.green) {
                                control.green = text;
                            } else if (modelData === control.blue) {
                                control.blue = text;
                            }
                        }
                    }
                }
            }

            Item { }

            Repeater {
                model: ["#", "R", "G", "B"]
                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font: D.DTK.fontManager.t8
                    text: modelData
                }
            }
        }
    }

    footer: D.DialogButtonBox {
        alignment: Qt.AlignCenter
        standardButtons: DialogButtonBox.NoButton
        Button {
            text: qsTr("Cancel")
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
        Button {
            text: qsTr("Save")
            highlighted: true
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
    }
}
