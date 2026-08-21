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
import org.deepin.dcc 1.0

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

    topPadding: 10
    leftPadding: 10
    rightPadding: 10
    bottomPadding: 10

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

            D.IconButton {
                id: eyeDropperButton
                icon.name: "color_extractor"
                icon.width: 20
                textColor: D.Palette {
                    normal: Qt.rgba(0, 0, 0, 1)
                    normalDark: Qt.rgba(1, 1, 1, 1)
                }
                
                flat: true
                visible: true
                Layout.preferredWidth: 22
                Layout.preferredHeight: 22
                opacity: 0.6

                background: Rectangle {
                    implicitWidth: 22
                    implicitHeight: 22
                    radius: 4
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

            Slider {
                id: hueSlider
                orientation: Qt.Horizontal
                value: control.hue
                implicitHeight: 16
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
                        anchors.topMargin: 3
                        anchors.bottomMargin: 3
                        radius: 2
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

            Rectangle {
                Layout.preferredHeight: 28
                Layout.preferredWidth: 28
                color: control.color
                border.width: 1
                border.color: Qt.rgba(0, 0, 0, 0.1)
                radius: 3
            }

            D.TextField {
                text: control.color.toString().substring(1).toUpperCase()
                Layout.preferredWidth: 74
                Layout.preferredHeight: 28
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font {
                    family: D.DTK.fontManager.t8.family
                    pointSize: D.DTK.fontManager.t8.pointSize
                    weight: Font.Medium
                }
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
                    Layout.preferredWidth: 44
                    Layout.preferredHeight: 28
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    inputMethodHints: Qt.ImhDigitsOnly
                    validator: IntValidator { bottom: 0; top: 255 }
                    font {
                        family: D.DTK.fontManager.t8.family
                        pointSize: D.DTK.fontManager.t8.pointSize
                        weight: Font.Medium
                    }

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
                model: ["Hex", "R", "G", "B"]
                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font {
                        family: D.DTK.fontManager.t8.family
                        pointSize: D.DTK.fontManager.t8.pointSize
                        weight: Font.Medium
                    }
                    text: modelData
                }
            }
        }
    }

    footer: ColumnLayout {
        spacing: 10
        Rectangle {
            id: line
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            color: Qt.rgba(0, 0, 0, 0.08)
            height: 1
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
        }
        RowLayout {
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            Layout.bottomMargin: 10
            spacing: 9
            D.Button {
                text: qsTr("Cancel")
                Layout.fillWidth: true
                background: Rectangle {
                    color: Qt.rgba(0, 0, 0, 0.15)
                    radius: 6
                }
                onClicked: {
                    control.reject()
                }
            }
            D.Button {
                text: qsTr("Save")
                Layout.fillWidth: true
                background: Rectangle {
                    color: Qt.rgba(0, 0, 0, 0.15)
                    radius: 6
                }
                textColor: D.Palette {
                    normal {
                    common: D.DTK.makeColor(D.Color.Highlight)
                    }
                    normalDark: normal
                    hovered {
                        common: D.DTK.makeColor(D.Color.Highlight).lightness(+10)
                    }
                    hoveredDark: hovered
                }
                onClicked: {
                    control.accept()
                }
            }
        }   
    }
}
