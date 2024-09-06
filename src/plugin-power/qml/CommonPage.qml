// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    DccObject {
        name: "powerMode"
        parentName: "powerManage/common"
        displayName: qsTr("性能模式")
        weight: 10
        hasBackground: false
        pageType: DccObject.Item
        page: ColumnLayout {
            Label {
                Layout.leftMargin: 10
                font: D.DTK.fontManager.t4
                text: dccObj.displayName
            }
        }
    }

    DccObject {
        name: "powerModeGroup"
        parentName: "powerManage/common"
        weight: 100
        pageType: DccObject.Item
        page: PowerModeListview {
        }
    }

    DccObject {
        name: "powersaverSetting"
        parentName: "powerManage/common"
        displayName: qsTr("节能设置")
        weight: 200
        hasBackground: false
        pageType: DccObject.Item
        page: ColumnLayout {
            Label {
                Layout.leftMargin: 10
                font: D.DTK.fontManager.t4
                text: dccObj.displayName
            }
        }
    }

    DccObject {
        name: "LowPowerGroup"
        parentName: "powerManage/common"
        weight: 300
        pageType: DccObject.Item
        page: DccGroupView {}

        // TODO 需要判断电池

        DccObject {
            name: "lowPowerAutoSaveMode"
            parentName: "powerManage/common/LowPowerGroup"
            displayName: qsTr("低电量时自动开启节能模式")
            weight: 1
            visible: /*dccData.model.haveBettary*/ true
            pageType: DccObject.Editor
            page: D.Switch {
                checked: dccData.model.powerSavingModeAutoWhenQuantifyLow
                onCheckedChanged: {
                    dccData.worker.setPowerSavingModeAutoWhenQuantifyLow(checked)
                }
            }
        }

        DccObject {
            name: "lowPowerThreshold"
            parentName: "powerManage/common/LowPowerGroup"
            displayName: qsTr("低电量阈值")
            weight: 2
            pageType: DccObject.Editor
            page: D.ComboBox {
                width: 100
                model: [ "10%", "20%", "30%", "40%", "50%" ]
                flat: true
                currentIndex: dccData.model.powerSavingModeAutoBatteryPercentage / 10 - 1

                onCurrentIndexChanged: {
                    dccData.worker.setPowerSavingModeAutoBatteryPercentage((currentIndex + 1) * 10)
                }
            }
        }
    }

    DccObject {
        name: "savePowerGroup"
        parentName: "powerManage/common"
        weight: 400
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "batteryAutoSaveMode"
            parentName: "powerManage/common/savePowerGroup"
            displayName: qsTr("使用电池时自动开启节能模式")
            weight: 1
            pageType: DccObject.Editor
            page: D.Switch {
                checked: dccData.model.autoPowerSaveMode
                onCheckedChanged: {
                    dccData.worker.setPowerSavingModeAuto(checked)
                }
            }
        }

        DccObject {
            name: "saveModeReduceBrightness"
            parentName: "powerManage/common/savePowerGroup"
            displayName: qsTr("节能模式时降低屏幕亮度")
            weight: 2
            pageType: DccObject.Item
            page: ColumnLayout {
                Layout.fillHeight: true
                Label {
                    id: speedText
                    Layout.topMargin: 10
                    font: D.DTK.fontManager.t7
                    text: dccObj.displayName
                    Layout.leftMargin: 10
                }
                D.TipsSlider {
                    id: scrollSlider
                    readonly property var tips: [("10%"), ("20%"), ("30%"), ("40%")]
                    Layout.preferredHeight: 80
                    Layout.alignment: Qt.AlignCenter
                    Layout.margins: 10
                    Layout.fillWidth: true
                    slider.handleType: Slider.HandleType.ArrowBottom
                    slider.from: 10
                    slider.to: ticks.length * 10
                    slider.live: true
                    slider.stepSize: 10
                    slider.snapMode: Slider.SnapAlways
                    slider.value: dccData.model.powerSavingModeLowerBrightnessThreshold
                    ticks: [
                        D.SliderTipItem { text: scrollSlider.tips[0]; highlight: scrollSlider.slider.value === 10
                        },
                        D.SliderTipItem { text: scrollSlider.tips[1]; highlight: scrollSlider.slider.value === 20
                        },
                        D.SliderTipItem { text: scrollSlider.tips[2]; highlight: scrollSlider.slider.value === 30
                        },
                        D.SliderTipItem { text: scrollSlider.tips[3]; highlight: scrollSlider.slider.value === 40
                        }
                    ]

                    slider.onValueChanged: {
                        dccData.worker.setPowerSavingModeLowerBrightnessThreshold(slider.value)
                    }
                }
            }
        }
    }

    DccObject {
        name: "wakeupSetting"
        parentName: "powerManage/common"
        displayName: qsTr("唤醒设置")
        weight: 500
        hasBackground: false
        pageType: DccObject.Item
        page: ColumnLayout {
            Label {
                Layout.leftMargin: 10
                font: D.DTK.fontManager.t4
                text: dccObj.displayName
            }
        }
    }

    DccObject {
        name: "wakeUpGroup"
        parentName: "powerManage/common"
        weight: 600
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "StandbyRecNeedPwd"
            parentName: "powerManage/common/wakeUpGroup"
            displayName: qsTr("待机恢复时需要密码")
            weight: 1
            visible: dccData.model.canSuspend && dccData.model.isSuspend
            pageType: DccObject.Editor
            page: D.Switch {
                checked: dccData.model.sleepLock && !dccData.model.isNoPasswdLogin
                enabled: !dccData.model.isNoPasswdLogin
                onCheckedChanged: {
                    dccData.worker.setSleepLock(checked)
                }
            }
        }

        DccObject {
            name: "wakeupDisplyNeedPwd"
            parentName: "powerManage/common/wakeUpGroup"
            displayName: qsTr("唤醒显示器时需要密码")
            weight: 2
            pageType: DccObject.Editor
            page: D.Switch {
                checked: dccData.model.screenBlackLock && !dccData.model.isNoPasswdLogin
                enabled: !dccData.model.isNoPasswdLogin
                onCheckedChanged: {
                    dccData.worker.setScreenBlackLock(checked)
                }
            }
        }
    }

    DccObject {
        name: "powerSetting"
        parentName: "powerManage/common"
        displayName: qsTr("关机设置")
        weight: 700
        hasBackground: false
        pageType: DccObject.Item
        page: ColumnLayout {
            Label {
                Layout.leftMargin: 10
                font: D.DTK.fontManager.t4
                text: dccObj.displayName
            }
        }
    }

    DccObject {
        name: "powerGroup"
        parentName: "powerManage/common"
        weight: 800
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "timedPoweroff"
            parentName: "powerManage/common/powerGroup"
            displayName: qsTr("定时关机")
            weight: 1
            pageType: DccObject.Editor
            page: D.Switch {
                id: timedShutdownSwitch
            }
        }

        DccObject {
            name: "shutdownTime"
            parentName: "powerManage/common/powerGroup"
            visible: timedShutdownSwitch.checked
            displayName: qsTr("时间")
            weight: 2
            pageType: DccObject.Editor
            page: Row {
                spacing: 10
                D.SpinBox {
                    width: 60
                    from: 0
                    to: 23
                    value: 8
                    font.family: "DigifaceWide"
                    font.pointSize: 12
                    textFromValue: function (value, locale) {
                        return value < 10 ? "0" + Number(value) : value
                    }
                }
                Label {
                    text: ":"
                    font.pointSize: 14
                    verticalAlignment: Text.AlignVCenter
                }
                D.SpinBox {
                    width: 60
                    value: 24
                    from: 0
                    to: 59
                    font.family: "DigifaceWide"
                    font.pointSize: 12
                    textFromValue: function (value, locale) {
                        return value < 10 ? "0" + Number(value) : value
                    }
                }
            }
        }

        DccObject {
            name: "repeatDays"
            parentName: "powerManage/common/powerGroup"
            visible: timedShutdownSwitch.checked
            displayName: qsTr("重复")
            weight: 3
            pageType: DccObject.Editor
            page: D.ComboBox {
                width: 100
                model: [ "一次", "每天", "工作日", "自定义" ]
                flat: true
            }
        }
        DccObject {
            name: "repeatDaysEdit"
            parentName: "powerManage/common/powerGroup"
            visible: timedShutdownSwitch.checked
            weight: 4
            pageType: DccObject.Editor
            page: RowLayout {
                Label {
                    text: "星期一、星期二、星期三、星期四"
                }
                D.ToolButton {
                    icon.name: "action_edit"
                    icon.width: 12
                    icon.height: 12
                    implicitWidth: 24
                    implicitHeight: 24
                    background: Rectangle {
                        color: "transparent"
                        border.color: "transparent"
                        border.width: 0
                    }

                    onClicked: {
                        selectDayDialog.show()
                    }
                }

                D.DialogWindow {
                    id: selectDayDialog
                    width: 330
                    height: 400
                    ColumnLayout {
                        implicitWidth: parent.width
                        clip: true
                        ListView {
                            Layout.fillWidth: true
                            height: contentHeight
                            clip: true
                            model: ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"]
                            spacing: 2
                            delegate: D.ItemDelegate {
                                width: ListView.view.width
                                leftPadding: 10
                                rightPadding: 10
                                implicitHeight: 40
                                checkable: false
                                corners: getCornersForBackground(index, 7)
                                cascadeSelected: true
                                text: modelData
                                content: D.IconButton {
                                    icon.name: model.isChecked ? "qrc:/icons/deepin/builtin/actions/checked.png" : "qrc:/icons/deepin/builtin/actions/nocheck.png"
                                    icon.width: 24
                                    icon.height: 24
                                    implicitWidth: 36
                                    implicitHeight: 36
                                    background: Rectangle {
                                        color: "transparent"
                                        border.color: "transparent"
                                        border.width: 0
                                    }
                                }
                                background: DccListViewBackground {
                                    separatorVisible: true
                                    highlightEnable: false
                                }
                            }
                        }

                        RowLayout {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignHCenter
                            spacing: 10
                            D.Button {
                                text: "取消"
                                onClicked: {
                                    selectDayDialog.close()
                                }
                            }
                            D.Button {
                                text: "保存"
                                onClicked: {
                                    selectDayDialog.close()
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
