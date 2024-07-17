// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import org.deepin.dtk 1.0
Rectangle {

    readonly property int itemDelegateMaxWidth: parent.width
    readonly property int itemDelegateMaxheight: 40
    //    border.color: "white"
    color: "transparent"
    implicitHeight: control1.implicitHeight
    //height: parent.height
    width: itemDelegateMaxWidth
    anchors {
        left: parent.left
    }
    ColumnLayout {
        width: itemDelegateMaxWidth
        id: control1

        function toPercent(value) {
            return Number(value * 100).toFixed(0) + "%"
        }

        Text {
            font: DTK.fontManager.t4
            text: "输入"
            Layout.leftMargin: 10
        }

        Rectangle {
            border.color: "white"
            border.width: 1
            color: "white"
            width: itemDelegateMaxWidth
            radius: 10
            height: control.implicitHeight


            ColumnLayout {
                id: control
                width: itemDelegateMaxWidth

                spacing: 5
                RowLayout {
                    Layout.preferredWidth: parent.width
                    height: itemDelegateMaxheight
                    width: parent.width
                    Layout.alignment: Qt.AlignVCenter
                    visible: inPutCombo.count > 0
                    Text {
                        font: DTK.fontManager.t4
                        text: "输入音量"
                        Layout.alignment: Qt.AlignLeft
                        Layout.leftMargin: 10
                        opacity: 0.7
                    }
                    RowLayout {
                        Layout.alignment: Qt.AlignRight
                        Layout.rightMargin: 10
                        spacing: 10

                        Label {
                            Layout.alignment: Qt.AlignVCenter
                            Layout.topMargin: 2
                            font: DTK.fontManager.t7
                            text: control1.toPercent(voiceTipsSlider1.value)
                        }

                        IconButton {
                            icon.name: "qrc:/icons/deepin/builtin/actions/dcc_volume1_32px.svg"
                            icon.width: 24
                            icon.height: 24
                            implicitWidth: 24
                            background: Rectangle {
                                color: "transparent" // 背景透明
                                border.color: "transparent" // 边框透明
                                border.width: 0
                            }
                        }
                        Slider {
                            id: voiceTipsSlider1
                            Layout.alignment: Qt.AlignVCenter
                            Layout.fillWidth: false
                            handleType: Slider.HandleType.NoArrowHorizontal
                            highlightedPassedGroove: true
                            from: 0
                            stepSize: 0.00001
                            to: 1
                            value: dccData.model().microphoneVolume
                            Layout.topMargin: 12
                            onValueChanged: {
                                dccData.worker().setSourceVolume(voiceTipsSlider1.value)
                            }
                        }
                        IconButton {
                            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                            icon.name: "qrc:/icons/deepin/builtin/actions/dcc_volume3_32px.svg"
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

                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    color: "#f2f2f2"
                    height: 1
                    Layout.preferredWidth: control.width - 20
                    visible: inPutCombo.count > 0
                }

                RowLayout {
                    Layout.preferredWidth: parent.width
                    height: itemDelegateMaxheight
                    width: parent.width
                    Layout.alignment: Qt.AlignVCenter
                    visible: inPutCombo.count > 0
                    Text {
                        font: DTK.fontManager.t4
                        text: "反馈音量"
                        Layout.alignment: Qt.AlignLeft
                        Layout.leftMargin: 10
                        opacity: 0.7
                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignRight
                        Layout.rightMargin: 10
                        spacing: 10

                        DciIcon {
                            Layout.topMargin: 2
                            name: "entry_voice"
                            sourceSize: Qt.size(16, 16)
                        }
                        Slider {
                            Layout.fillWidth: false
                            handleType: Slider.HandleType.NoArrowHorizontal
                            highlightedPassedGroove: true
                            value: dccData.model().microphoneFeedback
                            Layout.topMargin: 12
                        }
                        DciIcon {
                            Layout.rightMargin: 10
                            Layout.topMargin: 2
                            name: "entry_voice"
                            sourceSize: Qt.size(16, 16)
                        }
                    }
                }
                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    color: "#f2f2f2"
                    height: 1
                    Layout.preferredWidth: control.width - 20
                    visible: inPutCombo.count > 0
                }

                RowLayout {
                    Layout.preferredWidth: parent.width
                    height: itemDelegateMaxheight
                    width: parent.width
                    visible: inPutCombo.count > 0

                    Text {
                        Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                        font: DTK.fontManager.t4
                        text: "噪音抑制"
                        Layout.leftMargin: 10
                        opacity: 0.7
                    }

                    Switch {
                        Layout.alignment: Qt.AlignRight | Qt.AlignTop

                        checked: dccData.model().reduceNoise
                        onCheckedChanged: {
                            dccData.worker().setReduceNoise(checked)
                        }
                    }
                }

                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    color: "#f2f2f2"
                    height: 1
                    Layout.preferredWidth: control.width - 20
                    visible: inPutCombo.count > 0
                }


                RowLayout {
                    Layout.preferredWidth: parent.width
                    height: itemDelegateMaxheight
                    width: parent.width
                    Layout.alignment: Qt.AlignVCenter
                    Layout.bottomMargin: 10

                    Text {
                        Layout.alignment: Qt.AlignLeft
                        Layout.leftMargin: 10
                        Layout.topMargin: 10
                        font: DTK.fontManager.t4
                        text: "输入设备"
                        opacity: 0.7
                    }
                    ComboBox {
                        id: inPutCombo
                        Layout.alignment: Qt.AlignRight
                        Layout.rightMargin: 10
                        Layout.topMargin: 10
                        currentIndex: dccData.model().inPutPortComboIndex
                        flat: true
                        model: dccData.model().inPutPortCombo
                    }
                }
            }
        }

        Item {
            width: parent.width
            height: 20
        }
    }
}
