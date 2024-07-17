// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import org.deepin.dtk 1.0

import org.deepin.dcc 1.0

Rectangle {

    readonly property int itemDelegateMaxWidth: parent.width
   // color: "transparent"
    color: Qt.rgba(0, 0, 0, 0) // 设置为完全透明

//    height: 350
    implicitHeight: control.implicitHeight
    width: itemDelegateMaxWidth
    anchors {
        left: parent.left
    }

    ColumnLayout {
        id: control
        function toPercent(value: string) {
            return Number(value * 100).toFixed(0) + "%"
        }

        width: itemDelegateMaxWidth

        Text {
            height: 20
            font: DTK.fontManager.t4
            text: "输出"
            Layout.leftMargin: 10
            color: "black"

        }

        Rectangle {
            id: control2
            border.color: "white"
            border.width: 1
            color: "white"

            width: itemDelegateMaxWidth
            radius: 10
            height: control1.implicitHeight
            ColumnLayout {
                id: control1
                width: itemDelegateMaxWidth -20

                spacing: 5
                anchors {
                    left: parent.left
                    leftMargin: 10 // 设置左边间距为20
                    rightMargin: 10
                }

                RowLayout {
                    width: itemDelegateMaxWidth
                    height: 30
                    Layout.preferredWidth: itemDelegateMaxWidth
                    Text {
                        font: DTK.fontManager.t4
                        textFormat: "AutoText"
                        text: "输出音量"
                        verticalAlignment: "AlignVCenter"
                        opacity: 0.7
                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignRight
                        visible: outPutCombo.count > 0
                        Label {
                            Layout.alignment: Qt.AlignTop
                            Layout.topMargin: 2
                            font: DTK.fontManager.t7
                            text: control.toPercent(voiceTipsSlider.value)
                        }

                        IconButton{
                            Layout.alignment: Qt.AlignTop
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
                            id: voiceTipsSlider
                            Layout.alignment: Qt.AlignTop
                            Layout.fillWidth: false
                            handleType: Slider.HandleType.NoArrowHorizontal
                            highlightedPassedGroove: true
                            value: dccData.model().speakerVolume
                            onValueChanged: {
                                dccData.worker().setSinkVolume(voiceTipsSlider.value)
                            }
                        }

                        IconButton{
                            Layout.alignment: Qt.AlignTop
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
                    color: "#f2f2f2"
                    height: 1
                    Layout.preferredWidth: control1.width
                }

                RowLayout {
                    id: aaa
                    implicitHeight: 50
                    width: itemDelegateMaxWidth
                    Layout.preferredWidth: itemDelegateMaxWidth
                    visible: outPutCombo.count > 0
                    ColumnLayout {
                        Layout.alignment: Qt.AlignLeft
                        Text {
                            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                            font: DTK.fontManager.t4
                            text: "音量增强"
                            opacity: 0.7
                        }
                        Text {
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            font: DTK.fontManager.t5
                            opacity: 0.5
                            text: "音量大于100%时可能会导致音效失真,同时损害您的音频输出设备"
                        }
                    }

                    Switch {
                        Layout.alignment: Qt.AlignRight
                        checked: dccData.model().increaseVolume

                        onCheckedChanged: {
                            dccData.worker().setIncreaseVolume(checked)
                            if (checked) {
                                voiceTipsSlider.to = 1.5
                            } else {
                                voiceTipsSlider.to = 1
                            }
                        }
                    }
                }

                Rectangle {
                    color: "#f2f2f2"
                    height: 1
                    Layout.preferredWidth: control1.width
                }

                RowLayout {
                    Layout.preferredWidth: itemDelegateMaxWidth
                    implicitHeight: 20
                    visible: outPutCombo.count > 0

                    Item {
                        Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                        Layout.topMargin: 15
                        Text {
                            font: DTK.fontManager.t4
                            text: "音量平衡"
                            opacity: 0.7
                        }
                    }

                    RowLayout {
                        spacing: 10
                        Layout.alignment: Qt.AlignRight | Qt.AlignTop
                        Layout.rightMargin: 10
                        Text {
                            Layout.alignment: Qt.AlignTop
                            font: DTK.fontManager.t7
                            Layout.topMargin: 2
                            text: "左"
                        }
                        Slider {
                            id: balanceSlider
                            Layout.fillWidth: false
                            from: -1
                            handleType: Slider.HandleType.ArrowBottom
                            highlightedPassedGroove: true
                            stepSize: 0.00001
                            to: 1
                            value: dccData.model().speakerBalance

                            onValueChanged: {
                                dccData.worker().setSinkBalance(balanceSlider.value)
                            }
                        }
                        Text {
                            Layout.alignment: Qt.AlignRight | Qt.AlignTop
                            Layout.topMargin: 2
                            font: DTK.fontManager.t7
                            text: "右"
                        }
                    }
                }

                Rectangle {
                    color: "#f2f2f2"
                    height: 1
                    Layout.preferredWidth: control1.width
                }

                RowLayout {
                    implicitHeight: 60
                    width: itemDelegateMaxWidth
                    Layout.preferredWidth: itemDelegateMaxWidth
                    visible: outPutCombo.count > 0
                    ColumnLayout {
                        Layout.alignment: Qt.AlignLeft
                        Text {
                            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                            font: DTK.fontManager.t4
                            text: "插拔管理"
                            opacity: 0.7
                        }
                        Text {
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            font: DTK.fontManager.t7
                            opacity: 0.7
                            text: "外设插拔时音频输出是否自动暂停"
                        }
                    }
                    Switch {
                        Layout.alignment: Qt.AlignRight
                        checked: dccData.model().pausePlayer

                        onCheckedChanged: {
                            dccData.worker().setPausePlayer(checked)
                        }
                    }


                    // Item {
                    //     Layout.alignment: Qt.AlignTop | Qt.AlignRight
                    //     Layout.fillWidth: true
                    //     Layout.rightMargin: 10
                    //     Layout.topMargin: 20

                    //     Switch {
                    //         checked: dccData.model().pausePlayer
                    //         onCheckedChanged: {
                    //             dccData.worker().setPausePlayer(checked)
                    //         }
                    //     }
                    // }
                }

                Rectangle {
                    color: "#f2f2f2"
                    height: 1
                    Layout.preferredWidth: control1.width
                }

                RowLayout {
                   // spacing: 10
                    width: itemDelegateMaxWidth
                    Layout.preferredWidth: itemDelegateMaxWidth
                    Layout.bottomMargin: 10
                    Layout.alignment: Qt.AlignVCenter
                    implicitHeight: 30

                    Text {
                        Layout.alignment: Qt.AlignLeft
                        Layout.topMargin: 10
                        font: DTK.fontManager.t4
                        text: "输出设备"
                        opacity: 0.7
                    }
                    ComboBox {
                        id: outPutCombo
                        Layout.alignment: Qt.AlignRight
                        Layout.rightMargin: 10
                        flat: true
                        currentIndex: dccData.model().outPutPortComboIndex
                        model: dccData.model().outPutPortCombo
                        // background: {
                        //     color: "white"
                        // }
                        onCurrentIndexChanged: {
                            console.log("Selected index:", currentIndex)
                        }
                    }
                }
            }
        }

        Item {
            width: parent.width
            implicitHeight: 40
        }
    }
}
