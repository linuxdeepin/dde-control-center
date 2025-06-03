// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Templates as T
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

import org.deepin.dcc 1.0

DccObject {
    id: root

    function toPercent(value) {
        return Number(value * 100).toFixed(0) + "%"
    }

    DccTitleObject {
        name: "inPut"
        parentName: "sound/inPut"
        displayName: qsTr("Input")
        weight: 10
    }

    DccObject {
        name: "noIntput"
        parentName: "sound/inPut"
        weight: 20
        pageType: DccObject.Item
        backgroundType: DccObject.Normal
        visible: dccData.model().inPutPortCombo.length === 0
        page: Column {
            Label {
                height: 100
                width: parent.width
                Layout.leftMargin: 10
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font: D.DTK.fontManager.t8
                text: qsTr("No input device for sound found")
            }
        }
    }

    DccObject {
        name: "inputGroup"
        parentName: "sound/inPut"
        weight: 30
        pageType: DccObject.Item
        page: DccGroupView {}
        visible: dccData.model().inPutPortCombo.length !== 0
        DccObject{
            name: "inputVolume"
            parentName: "sound/inPut/inputGroup"
            displayName: qsTr("Input Volume")
            weight: 10
            pageType: DccObject.Editor
            page: RowLayout {
                Layout.alignment: Qt.AlignRight

                Label {
                    Layout.alignment: Qt.AlignVCenter
                    font: D.DTK.fontManager.t7
                    text: root.toPercent(voiceTipsSlider1.value)
                }

                D.IconButton {
                    icon.name: dccData.model().microphoneOn ? "sound_off" : "small_volume"
                    icon.width: 16
                    icon.height: 16
                    implicitWidth: 24

                    flat: !hovered

                    onClicked: {
                        dccData.worker().setSourceMute()
                    }
                }
                Slider {
                    id: voiceTipsSlider1
                    Layout.alignment: Qt.AlignVCenter
                    implicitHeight: 24
                    handleType: Slider.HandleType.NoArrowHorizontal
                    highlightedPassedGroove: true
                    from: 0
                    stepSize: 0.01
                    to: 1
                    value: dccData.model().microphoneVolume
                    onPressedChanged: {
                        if (!pressed && voiceTipsSlider1.value != dccData.model().microphoneVolume) {
                            dccData.worker().setSourceVolume(voiceTipsSlider1.value)
                        }
                    }
                }
                D.IconButton {
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    icon.name: "big_volume"
                    icon.width: 16
                    icon.height: 16
                    implicitWidth: 24
                    background: Rectangle {
                        color: "transparent" // 背景透明
                        border.color: "transparent" // 边框透明
                        border.width: 0
                    }
                }
            }
        }

        DccObject{
            name: "microphoneFeedback"
            parentName: "sound/inPut/inputGroup"
            displayName: qsTr("Input Level")
            weight: 20
            pageType: DccObject.Editor
            page: RowLayout {
                Layout.alignment: Qt.AlignRight

                D.DciIcon {
                    name: "small_volume"
                    palette: D.DTK.makeIconPalette(parent.palette)
                    sourceSize: Qt.size(16, 16)
                    implicitWidth: 24
                    Layout.rightMargin: 4
                }
                Item {
                    Layout.fillWidth: false
                    implicitHeight: slider.implicitHeight
                    implicitWidth: slider.implicitWidth

                    Slider {
                        id: slider
                        anchors.fill: parent
                        handleType: -2
                        highlightedPassedGroove: true
                        value: dccData.model().microphoneFeedback
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }
                D.DciIcon {
                    name: "big_volume"
                    palette: D.DTK.makeIconPalette(parent.palette)
                    sourceSize: Qt.size(16, 16)
                    implicitWidth: 24
                    Layout.rightMargin: 4
                    Layout.leftMargin: 4
                }
            }
        }

        DccObject {
            name: "reduceNoise"
            parentName: "sound/inPut/inputGroup"
            displayName: qsTr("Automatic Noise Suppression")
            weight: 30
            pageType: DccObject.Editor
            page: Switch {
                Layout.alignment: Qt.AlignRight | Qt.AlignTop

                checked: dccData.model().reduceNoise
                onCheckedChanged: {
                    if (dccData.model().reduceNoise !== checked) {
                        dccData.worker().setReduceNoise(checked)
                    }
                }
            }
        }

        DccObject {
            name: "inputDevice"
            parentName: "sound/inPut/inputGroup"
            displayName: qsTr("Input Devices")
            weight: 40
            pageType: DccObject.Editor
            page: ComboBox {
                id: control
                Layout.alignment: Qt.AlignRight
                Layout.rightMargin: 10
                currentIndex: dccData.model().inPutPortComboIndex
                flat: true
                textRole: "name"
                model: dccData.model().soundInputDeviceModel()
                enabled: dccData.model().inPutPortComboEnable
                implicitWidth: 300

                contentItem: RowLayout {
                    spacing: DS.Style.comboBox.spacing

                    Loader {
                        property string iconName: (control.iconNameRole && model.get(control.currentIndex)[control.iconNameRole] !== undefined)
                                                  ? model.get(control.currentIndex)[control.iconNameRole] : null
                        active: iconName

                        sourceComponent: D.DciIcon {
                            palette: DTK.makeIconPalette(control.palette)
                            mode: control.D.ColorSelector.controlState
                            theme: control.D.ColorSelector.controlTheme
                            name: iconName
                            sourceSize: Qt.size(DS.Style.comboBox.iconSize, DS.Style.comboBox.iconSize)
                            fallbackToQIcon: true
                        }
                    }

                    T.TextField {
                        id: textField

                        function getDisplayText() {
                            return control.editable ? control.editText : fm.elidedText(control.displayText,
                                                Text.ElideRight, control.implicitWidth - DS.Style.comboBox.iconSize - DS.Style.comboBox.spacing * 4)
                        }

                        FontMetrics {
                            id: fm
                            font: textField.font
                            onFontChanged: {
                                textField.text = textField.getDisplayText()
                            }
                        }
                        Connections {
                            target: control
                            function onDisplayTextChanged() {
                                textField.text = textField.getDisplayText()
                            }
                        }

                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.rightMargin: DS.Style.comboBox.spacing
                        text: getDisplayText()

                        enabled: control.editable
                        autoScroll: control.editable
                        readOnly: control.down
                        inputMethodHints: control.inputMethodHints
                        validator: control.validator
                        selectByMouse: true

                        color: control.editable ? control.palette.text : control.palette.buttonText
                        selectionColor: control.palette.highlight
                        selectedTextColor: control.palette.highlightedText
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: control.horizontalAlignment

                        ToolTip {
                            visible: !control.editable && textField.text !== control.displayText && textField.hovered
                            text: control.displayText
                            delay: 500
                        }
                    }
                }

                delegate: MenuItem {
                    id: menuItem
                    useIndicatorPadding: true
                    width: control.width
                    text: model.name
                    visible: model.isEnabled
                    icon.name: (control.iconNameRole && model[control.iconNameRole] !== undefined) ? model[control.iconNameRole] : null
                    highlighted: control.highlightedIndex === index
                    hoverEnabled: control.hoverEnabled
                    autoExclusive: true
                    checked: control.currentIndex === index
                    implicitHeight: visible ? DS.Style.control.implicitHeight(menuItem) : 0

                    onClicked: {
                        dccData.worker().setActivePort(index, 2)
                    }
                }
            }
        }
    }
}
