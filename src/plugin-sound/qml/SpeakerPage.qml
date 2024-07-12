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

    color: "transparent"
    height: 700
    width: parent.width -80
    anchors {
        left: parent.left
        leftMargin: 40 // 设置左边间距为20
        rightMargin: 40
    }

    ColumnLayout {
        id: control

        function toPercent(value) {
            return Number(value * 100).toFixed(0) + "%"
        }

        //    implicitWidth: parent.width
        Layout.preferredHeight: parent.height
        spacing: 10
        width: parent.width - 40

        Text {
            font: DTK.fontManager.t5
            text: "输出"
            Layout.leftMargin: 10
        }


        Rectangle {
            border.color: "black"
            border.width: 1
            color: "white"
            height: 350
            width: parent.width
            radius: 10

            ColumnLayout {
                id: control1
                width: parent.width -20
                height: parent.height
                Layout.preferredHeight: parent.height
                anchors {
                    left: parent.left
                    leftMargin: 10 // 设置左边间距为20
                    rightMargin: 10
                }

                RowLayout {
                    width: parent.width
                    height: 30
                    Layout.preferredWidth: parent.width
                    Text {
                        font: DTK.fontManager.t6
                        text: "输出音量"
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    }

                    RowLayout {
                        height: parent.height
                        Layout.alignment: Qt.AlignRight

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
                    color: "black"
                    height: 1
                    Layout.preferredWidth: control1.width
                }

                RowLayout {
                    id: aaa
                    height: 60
                    width: parent.width
                    Layout.preferredWidth: parent.width
                    ColumnLayout {
                        id: sss
                        Layout.alignment: Qt.AlignLeft
                        height: parent.height
                        Text {
                            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                            font: DTK.fontManager.t6
                            text: "音量增强"
                        }
                        Text {
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            font: DTK.fontManager.t7
                            opacity: 0.7
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
                    color: "black"
                    height: 1
                    Layout.preferredWidth: control1.width
                }

                RowLayout {
                    Layout.preferredWidth: parent.width
                    height: 20

                    Item {
                        Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                        Layout.topMargin: 15
                        Text {
                            font: DTK.fontManager.t6
                            text: "音量平衡"
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

                // Rectangle {
                //     Layout.alignment: Qt.AlignHCenter
                //     color: "black"
                //     height: 1
                //     width: parent.width - 20
                // }
                //
                // RowLayout {
                //     height: 60
                //     width: parent.width
                //
                //     ColumnLayout {
                //         Layout.alignment: Qt.AlignLeft
                //         Layout.leftMargin: 10
                //         Text {
                //             Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                //             font: DTK.fontManager.t6
                //             text: "单声道音频"
                //         }
                //         Text {
                //             Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                //             font: DTK.fontManager.t7
                //             opacity: 0.7
                //             text: "将左声道和右声道合并为一个声道"
                //         }
                //     }
                //
                //     Item {
                //         Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                //         Layout.fillWidth: true
                //         Layout.leftMargin: 10
                //         Layout.topMargin: 10
                //
                //     }
                //     Item {
                //         Layout.alignment: Qt.AlignTop
                //         Layout.fillWidth: true
                //         Layout.rightMargin: 10
                //         Layout.topMargin: 20
                //
                //         Switch {
                //             anchors.right: parent.right
                //             checked: dccData.model().increaseVolume
                //         }
                //     }
                // }

                Rectangle {
                    color: "black"
                    height: 1
                    Layout.preferredWidth: control1.width
                }

                RowLayout {
                    height: 60
                    width: parent.width
                    Layout.preferredWidth: parent.width

                    ColumnLayout {
                        Layout.alignment: Qt.AlignLeft
                        height: parent.height
                        Text {
                            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                            font: DTK.fontManager.t6
                            text: "插拔管理"
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
                    color: "black"
                    height: 1
                    Layout.preferredWidth: control1.width
                }

                RowLayout {
                    spacing: 10
                    width: parent.width
                    Layout.preferredWidth: parent.width
                    Layout.bottomMargin: 10
                    Layout.alignment: Qt.AlignVCenter

                    Text {
                        Layout.alignment: Qt.AlignLeft
                        Layout.topMargin: 10
                        font: DTK.fontManager.t6
                        text: "输出设备"
                    }
                    ComboBox {
                        Layout.alignment: Qt.AlignRight
                        Layout.rightMargin: 10
                        flat: true
                        currentIndex: dccData.model().outPutPortComboIndex
                        model: dccData.model().outPutPortCombo
                        background: {
                            color: "white"
                        }
                        onCurrentIndexChanged: {
                            console.log("Selected index:", currentIndex)
                        }
                    }
                }
            }
        }

        MicrophonePage {
            width: parent.width
        }
    }
}
