// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import org.deepin.dtk 1.0
import org.deepin.dtk.style 1.0 as DS

import org.deepin.dcc 1.0

DccObject {
    id: root

    function toPercent(value: string) {
        return Number(value * 100).toFixed(0) + "%"
    }

    DccObject {
        name: "inPut"
        parentName: "sound/inPut"
        displayName: qsTr("Input")
        weight: 10
        pageType: DccObject.Item
        page: ColumnLayout {
            Label {
                Layout.leftMargin: 10
                font: DTK.fontManager.t4
                text: dccObj.displayName
            }
        }
    }

    DccObject {
        name: "noIntput"
        parentName: "sound/inPut"
        weight: 20
        pageType: DccObject.Item
        backgroundType: DccObject.Normal
        visible: dccData.model().inPutPortCombo.length === 0
        page: Column {
            width: parent.width
            Label {
                height: 100
                width: parent.width
                Layout.leftMargin: 10
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font: DTK.fontManager.t8
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
                    font: DTK.fontManager.t7
                    text: root.toPercent(voiceTipsSlider1.value)
                }

                IconButton {
                    icon.name: dccData.model().microphoneOn ? "qrc:/icons/deepin/builtin/icons/sound_off.dci" : "qrc:/icons/deepin/builtin/icons/small_volume.dci"
                    icon.width: 24
                    icon.height: 24
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
                    stepSize: 0.00001
                    to: 1
                    value: dccData.model().microphoneVolume
                    onValueChanged: {
                        if (voiceTipsSlider1.value != dccData.model().microphoneVolume) {
                            dccData.worker().setSourceVolume(voiceTipsSlider1.value)
                        }
                    }
                }
                IconButton {
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    icon.name: "qrc:/icons/deepin/builtin/icons/big_volume.dci"
                    icon.width: 24
                    icon.height: 24
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

                DciIcon {
                    name: "qrc:/icons/deepin/builtin/icons/small_volume.dci"
                    sourceSize: Qt.size(24, 24)
                    implicitWidth: 24
                    Layout.rightMargin: 4
                }
                Slider {
                    Layout.fillWidth: false
                   // handleType: Slider.HandleType.NoArrowHorizontal
                    handleType: -2
                    highlightedPassedGroove: true
                    implicitHeight: 24
                    value: dccData.model().microphoneFeedback
                }
                DciIcon {
                    name: "qrc:/icons/deepin/builtin/icons/big_volume.dci"
                    sourceSize: Qt.size(24, 24)
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
                    dccData.worker().setReduceNoise(checked)
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
