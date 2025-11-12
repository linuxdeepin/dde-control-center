// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtQuick.Templates as T
import org.deepin.dtk 1.0
import org.deepin.dtk.style 1.0 as DS

import org.deepin.dcc 1.0

import SoundDeviceModel 1.0

DccObject {
    id: root
    function toPercent(value) {
        return Number(value * 100).toFixed(0) + "%"
    }
    DccTitleObject {
        name: "output"
        parentName: "sound/outPut"
        displayName: qsTr("Output")
        weight: 10
    }

    DccObject {
        name: "noOutput"
        parentName: "sound/outPut"
        weight: 20
        pageType: DccObject.Item
        backgroundType: DccObject.Normal
        visible: dccData.model().outPutPortCombo.length === 0
        page: Column {
            width: parent.width
            Label {
                height: 100
                width: parent.width
                Layout.leftMargin: 10
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font: DTK.fontManager.t4
                text: qsTr("No output device for sound found")
            }
        }
    }

    DccObject {
        name: "outputGroup"
        parentName: "sound/outPut"
        weight: 30
        visible: dccData.model().outPutPortCombo.length !== 0
        pageType: DccObject.Item
        page: DccGroupView { }
        DccObject {
            name: "outputVolume"
            parentName: "sound/outPut/outputGroup"
            displayName: qsTr("Output Volume")
            weight: 10
            pageType: DccObject.Editor
            page: RowLayout {
                Label {
                    font: DTK.fontManager.t10
                    color: Qt.rgba(palette.text.r, palette.text.g, palette.text.b, 0.5)
                    text: root.toPercent(voiceTipsSlider.value)
                }
                ActionButton {
                    Layout.alignment: Qt.AlignVCenter
                    icon {
                        name: dccData.model().speakerOn ? "sound_off" : "small_volume"
                        width: 16
                        height: 16
                    }
                    palette.windowText: ColorSelector.textColor
                    flat: !hovered
                    implicitWidth: 24
                    implicitHeight: 24
                    hoverEnabled: enabled
                    background: Rectangle {
                        property Palette pressedColor: Palette {
                            normal: Qt.rgba(0, 0, 0, 0.2)
                            normalDark: Qt.rgba(1, 1, 1, 0.25)
                        }
                        property Palette hoveredColor: Palette {
                            normal: Qt.rgba(0, 0, 0, 0.1)
                            normalDark: Qt.rgba(1, 1, 1, 0.1)
                        }
                        radius: DS.Style.control.radius
                        color: parent.pressed ? ColorSelector.pressedColor : (parent.hovered ? ColorSelector.hoveredColor : "transparent")
                        border {
                            color: parent.palette.highlight
                            width: parent.visualFocus ? DS.Style.control.focusBorderWidth : 0
                        }
                    }

                    onClicked: {
                        dccData.worker().setSinkMute()
                    }
                }
                Slider {
                    id: voiceTipsSlider
                    Layout.alignment: Qt.AlignVCenter
                    implicitHeight: 24
                    handleType: Slider.HandleType.NoArrowHorizontal
                    highlightedPassedGroove: true
                    value: dccData.model().speakerVolume
                    to: dccData.model().increaseVolume ? 1.5 : 1.0
                    stepSize: 0.01
                    onPressedChanged: {
                        if (!pressed) {
                            dccData.worker().setSinkVolume(voiceTipsSlider.value)
                        }
                    }
                }
                IconButton {
                    icon {
                        name: "big_volume"
                        width: 16
                        height: 16
                    }
                    implicitWidth: 24
                    background: Rectangle {
                        color: "transparent" // 背景透明
                        border.color: "transparent" // 边框透明
                        border.width: 0
                    }
                }
            }
        }
        DccObject {
            name: "volumeEnhancement"
            parentName: "sound/outPut/outputGroup"
            displayName: qsTr("Volume Boost")
            description: qsTr("If the volume is louder than 100%, it may distort audio and be harmful to output devices")
            weight: 20
            pageType: DccObject.Editor
            page: Switch {
                Layout.alignment: Qt.AlignRight
                checked: dccData.model().increaseVolume
                onCheckedChanged: {
                    if (dccData.model().increaseVolume !== checked) {
                        dccData.worker().setIncreaseVolume(checked)
                    }
                }
            }
        }
        DccObject {
            name: "volumeBalance"
            parentName: "sound/outPut/outputGroup"
            displayName: qsTr("Left Right Balance")
            weight: 40
            pageType: DccObject.Editor
            visible: !dccData.model().audioMono &&
                (!dccData.model().showBluetoothMode ||
                !dccData.model().outPutPortCombo[dccData.model().outPutPortComboIndex].toLowerCase().startsWith("handsfree"))
            page: RowLayout {
                Label {
                    Layout.alignment: Qt.AlignVCenter
                    font: DTK.fontManager.t7
                    Layout.topMargin: 2
                    text: qsTr("Left")
                }
                Slider {
                    id: balanceSlider
                    Layout.alignment: Qt.AlignVCenter
                    implicitHeight: 24
                    from: -1
                    handleType: Slider.HandleType.ArrowBottom
                    highlightedPassedGroove: true
                    stepSize: 0.01
                    to: 1
                    value: dccData.model().speakerBalance

                    onPressedChanged: {
                        if (!pressed) {
                            dccData.worker().setSinkBalance(balanceSlider.value)
                        }
                    }
                }
                Label {
                    Layout.alignment: Qt.AlignVCenter
                    Layout.topMargin: 2
                    font: DTK.fontManager.t7
                    text: qsTr("Right")
                }
            }
        }
        DccObject {
            name: "monoAudio"
            parentName: "sound/outPut/outputGroup"
            displayName: qsTr("Mono Audio")
            description: qsTr("Merge left and right channels into a single channel")
            weight: 30
            pageType: DccObject.Editor
            page: Switch {
                checked: dccData.model().audioMono
                onCheckedChanged: {
                    if (dccData.model().audioMono !== checked) {
                        dccData.worker().setAudioMono(checked)
                    }
                }
            }
        }
        DccObject {
            name: "plugAndUnplugManagement"
            parentName: "sound/outPut/outputGroup"
            displayName: qsTr("Auto Pause")
            description: qsTr("Whether the audio will be automatically paused when the current audio device is unplugged")
            weight: 50
            pageType: DccObject.Editor
            page: Switch {
                checked: dccData.model().pausePlayer
                onCheckedChanged: {
                    if (dccData.model().pausePlayer !== checked) {
                        dccData.worker().setPausePlayer(checked)
                    }
                }
            }
        }
        DccObject {
            name: "outputDevice"
            parentName: "sound/outPut/outputGroup"
            displayName: qsTr("Output Device")
            weight: 60
            pageType: DccObject.Editor
            page: ComboBox {
                id: control
                Layout.alignment: Qt.AlignRight
                Layout.rightMargin: 10
                flat: true
                textRole: "name"
                currentIndex: dccData.model().outPutPortComboIndex
                model: dccData.model().soundOutputDeviceModel()
                enabled: dccData.model().outPutPortComboEnable
                implicitWidth: 300

                contentItem: RowLayout {
                    spacing: DS.Style.comboBox.spacing

                    Loader {
                        property string iconName: (control.iconNameRole && model.get(control.currentIndex)[control.iconNameRole] !== undefined)
                                                  ? model.get(control.currentIndex)[control.iconNameRole] : null
                        active: iconName

                        sourceComponent: DciIcon {
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
                        implicitHeight: fm.height
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

                property bool isInitialized: false

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

                    readonly property real availableTextWidth: {
                        if (!contentItem)
                            return width - leftPadding - rightPadding
                        let textWidth = contentItem.width - contentItem.leftPadding - contentItem.rightPadding
                        if (icon.name) {
                            textWidth -= DS.Style.menu.item.iconSize.width + spacing
                        }
                        return textWidth
                    }
                    FontMetrics {
                        id: fontMetrics
                        font: menuItem.font
                    }
                    ToolTip {
                        visible: menuItem.hovered && fontMetrics.advanceWidth(model.name) > menuItem.availableTextWidth
                        text: model.name
                        delay: 500
                    }

                    onClicked: {
                        dccData.worker().setActivePort(index, 1)
                    }
                }
            }
        }

        DccObject {
            name: "bluetoothMode"
            parentName: "sound/outPut/outputGroup"
            displayName: qsTr("Mode")
            weight: 60
            pageType: DccObject.Editor
            visible: dccData.model().showBluetoothMode
            page: ComboBox {
                id: bluetoothControl
                Layout.alignment: Qt.AlignRight
                Layout.rightMargin: 10
                flat: true
                model: dccData.model().bluetoothModeOpts
                currentIndex: count > 0 ? Math.max(0, indexOfValue(dccData.model().currentBluetoothAudioMode)) : 0
                property bool isInitialized: false
                implicitWidth: 300

                contentItem: RowLayout {
                    spacing: DS.Style.comboBox.spacing

                    T.TextField {
                        id: textField

                        function getDisplayText() {
                            return bluetoothControl.editable ? bluetoothControl.editText : fm.elidedText(bluetoothControl.displayText,
                                                Text.ElideRight, bluetoothControl.implicitWidth - DS.Style.comboBox.spacing * 4)
                        }

                        FontMetrics {
                            id: fm
                            font: textField.font
                            onFontChanged: {
                                textField.text = textField.getDisplayText()
                            }
                        }
                        Connections {
                            target: bluetoothControl
                            function onDisplayTextChanged() {
                                textField.text = textField.getDisplayText()
                            }
                        }

                        Layout.fillWidth: true
                        implicitHeight: fm.height
                        Layout.rightMargin: DS.Style.comboBox.spacing
                        text: getDisplayText()

                        enabled: bluetoothControl.editable
                        autoScroll: bluetoothControl.editable
                        readOnly: bluetoothControl.down
                        inputMethodHints: bluetoothControl.inputMethodHints
                        validator: bluetoothControl.validator
                        selectByMouse: true

                        color: bluetoothControl.editable ? bluetoothControl.palette.text : bluetoothControl.palette.buttonText
                        selectionColor: bluetoothControl.palette.highlight
                        selectedTextColor: bluetoothControl.palette.highlightedText
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: bluetoothControl.horizontalAlignment

                        ToolTip {
                            visible: !bluetoothControl.editable && textField.text !== bluetoothControl.displayText && textField.hovered
                            text: bluetoothControl.displayText
                            delay: 500
                        }
                    }
                }

                delegate: MenuItem {
                    id: menuItem
                    useIndicatorPadding: true
                    width: bluetoothControl.width
                    text: modelData
                    highlighted: bluetoothControl.highlightedIndex === index
                    hoverEnabled: bluetoothControl.hoverEnabled
                    autoExclusive: true
                    checked: bluetoothControl.currentIndex === index
                    implicitHeight: DS.Style.control.implicitHeight(menuItem)

                    readonly property real availableTextWidth: {
                        if (!contentItem)
                            return width - leftPadding - rightPadding
                        let textWidth = contentItem.width - contentItem.leftPadding - contentItem.rightPadding
                        if (icon.name) {
                            textWidth -= DS.Style.menu.item.iconSize.width + spacing
                        }
                        return textWidth
                    }
                    FontMetrics {
                        id: fontMetrics
                        font: menuItem.font
                    }
                    ToolTip {
                        visible: menuItem.hovered && fontMetrics.advanceWidth(modelData) > menuItem.availableTextWidth
                        text: modelData
                        delay: 500
                    }
                }
                
                Connections {
                    target: dccData.model()
                    function onBluetoothModeOptsChanged() { bluetoothControl.currentIndex = Math.max(0, bluetoothControl.indexOfValue(dccData.model().currentBluetoothAudioMode)) }
                    function onBluetoothModeChanged() { bluetoothControl.currentIndex = Math.max(0, bluetoothControl.indexOfValue(dccData.model().currentBluetoothAudioMode)) }
                }
                
                Component.onCompleted: { isInitialized = true }
                
                onCurrentIndexChanged: {
                    if (isInitialized && currentIndex >= 0 && currentIndex < count) {
                        dccData.worker().setBluetoothMode(valueAt(currentIndex))
                    }
                }
            }
        }
    }
}
