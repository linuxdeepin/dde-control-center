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

ColorDialogImpl {
    id: control
    color: "transparent"
    width: parent.width
    height: parent.height

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

    ColorDialogImpl.eyeDropperButton: eyeDropperButton
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
                flat: true
                visible: true
                Layout.preferredWidth: implicitHeight
            }

            D.TextField {
                text: control.color.toString().substring(1)
                readOnly: true
                Layout.preferredWidth: 70
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Repeater {
                model: [control.red, control.green, control.blue]
                D.TextField {
                    text: modelData
                    readOnly: true
                    Layout.fillWidth: true
                    Layout.preferredWidth: 70
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
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
