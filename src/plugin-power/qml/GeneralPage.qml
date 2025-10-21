// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    DccTitleObject {
        name: "powerPlansTitle"
        parentName: "power/general"
        displayName: qsTr("Power Plans")
        weight: 10
        visible: dccData.platformName() !== "wayland"
    }

    DccObject {
        name: "powerPlans"
        parentName: "power/general"
        weight: 100
        pageType: DccObject.Item
        visible: dccData.platformName() !== "wayland"
        page: DccGroupView {}
        PowerPlansListview {}
    }

    DccTitleObject {
        name: "powerSavingSettingsTitle"
        parentName: "power/general"
        displayName: qsTr("Power Saving Settings")
        weight: 200
        visible: dccData.platformName() !== "wayland"
    }

    DccObject {
        name: "powerSavingSettingsGroup"
        parentName: "power/general"
        weight: 300
        pageType: DccObject.Item
        visible: dccData.platformName() !== "wayland"
        page: DccGroupView {}

        DccObject {
            name: "autoPowerSavingOnLowBattery"
            parentName: "power/general/powerSavingSettingsGroup"
            displayName: qsTr("Auto power saving on low battery")
            weight: 1
            visible: dccData.model.haveBettary
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
            parentName: "power/general/powerSavingSettingsGroup"
            displayName: qsTr("Low battery threshold")
            weight: 2
            visible: dccData.model.haveBettary && dccData.model.powerSavingModeAutoWhenQuantifyLow
            pageType: DccObject.Editor
            page: D.ComboBox {
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
        name: "autoPowerSavingOnBattery"
        parentName: "power/general"
        displayName: qsTr("Auto power saving on battery")
        weight: 400
        backgroundType: DccObject.Normal
        visible: dccData.model.haveBettary && dccData.platformName() !== "wayland"
        pageType: DccObject.Editor
        page: D.Switch {
            checked: dccData.model.autoPowerSaveMode
            onCheckedChanged: {
                dccData.worker.setPowerSavingModeAuto(checked)
            }
        }
    }

    DccObject {
        name: "decreaseBrightness"
        parentName: "power/general"
        displayName: qsTr("Decrease screen brightness on power saver")
        weight: 450
        visible: dccData.platformName() !== "wayland"
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: ColumnLayout {
            Layout.fillHeight: true
            Label {
                Layout.topMargin: 10
                Layout.leftMargin: 14
                font: D.DTK.fontManager.t6
                text: dccObj.displayName
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
                    D.SliderTipItem { text: scrollSlider.tips[0]; highlight: scrollSlider.slider.value === 10 },
                    D.SliderTipItem { text: scrollSlider.tips[1]; highlight: scrollSlider.slider.value === 20 },
                    D.SliderTipItem { text: scrollSlider.tips[2]; highlight: scrollSlider.slider.value === 30 },
                    D.SliderTipItem { text: scrollSlider.tips[3]; highlight: scrollSlider.slider.value === 40 }
                ]

                slider.onValueChanged: {
                    dccData.worker.setPowerSavingModeLowerBrightnessThreshold(slider.value)
                }
            }
        }
    }

    DccTitleObject {
        name: "wakeupSettingsTitle"
        parentName: "power/general"
        displayName: qsTr("Wakeup Settings")
        weight: 500
    }

    DccObject {
        name: "wakeupSettingsGroup"
        parentName: "power/general"
        weight: 600
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "passwordIsRequiredToWakeUpTheComputer"
            parentName: "power/general/wakeupSettingsGroup"
            displayName: qsTr("Password is required to wake up the computer")
            weight: 1
            visible: dccData.model.canSuspend && dccData.model.isSuspend && !dccData.model.isVirtualEnvironment
            pageType: DccObject.Editor
            page: D.Switch {
                checked: dccData.model.sleepLock
                enabled: !dccData.model.isNoPasswdLogin
                onCheckedChanged: {
                    dccData.worker.setSleepLock(checked)
                }
            }
        }

        DccObject {
            name: "passwordIsRequiredToWakeUpTheMonitor"
            parentName: "power/general/wakeupSettingsGroup"
            displayName: qsTr("Password is required to wake up the monitor")
            weight: 2
            pageType: DccObject.Editor
            page: D.Switch {
                checked: dccData.model.screenBlackLock
                enabled: !dccData.model.isNoPasswdLogin
                onCheckedChanged: {
                    dccData.worker.setScreenBlackLock(checked)
                }
            }
        }
    }

    DccTitleObject {
        name: "shutdownSettingTitle"
        parentName: "power/general"
        displayName: qsTr("Shutdown Settings")
        weight: 700
        visible: dccData.model.enableScheduledShutdown !== "Hidden"
        enabled: dccData.model.enableScheduledShutdown !== "Disabled"
    }

    DccObject {
        name: "shutdownGroup"
        parentName: "power/general"
        weight: 800
        pageType: DccObject.Item
        visible: dccData.model.enableScheduledShutdown !== "Hidden"
        enabled: dccData.model.enableScheduledShutdown !== "Disabled"
        page: DccGroupView {}

        DccObject {
            name: "timedPoweroff"
            parentName: "power/general/shutdownGroup"
            displayName: qsTr("Scheduled Shutdown")
            weight: 1
            pageType: DccObject.Editor
            page: D.Switch {
                checked: dccData.model.scheduledShutdownState
                onCheckedChanged: {
                    dccData.worker.setScheduledShutdownState(checked)
                }
            }
        }

        DccObject {
            name: "poweroffTime"
            parentName: "power/general/shutdownGroup"
            visible: dccData.model.scheduledShutdownState
            displayName: qsTr("Time")
            weight: 2
            pageType: DccObject.Editor
            page: RowLayout {
                DccTimeRange {
                    id: timeRange
                    Layout.preferredWidth: 100
                    hour: dccData.model.shutdownTime.split(':')[0]
                    minute: dccData.model.shutdownTime.split(':')[1]

                    onTimeChanged: {
                        dccData.worker.setShutdownTime(timeRange.timeString)
                    }
                }
            }
        }

        DccObject {
            name: "repeatDays"
            parentName: "power/general/shutdownGroup"
            visible: dccData.model.scheduledShutdownState
            displayName: qsTr("Repeat")
            weight: 3
            pageType: DccObject.Editor
            page: D.ComboBox {
                id: shutdownRepetitionCombobox
                model: [ qsTr("Once"), qsTr("Every day"), qsTr("Working days"), qsTr("Custom Time") ]
                flat: true
                currentIndex: dccData.model.shutdownRepetition
                onCurrentIndexChanged: {
                    if (currentIndex === model.length - 1 && dccData.model.customShutdownWeekDays.length === 0) {
                        selectDayDialogforCombobox.show()
                    } else {
                        if (currentIndex === dccData.model.shutdownRepetition) {
                            return
                        }
                        dccData.worker.setShutdownRepetition(currentIndex)
                    }
                }
                ScheduledShutdownDialog {
                    id: selectDayDialogforCombobox

                    onCancel: {
                        // 通过Q_INVOKABLE方法触发信号更新，直接赋值currentIndex会导致属性绑定失效
                        dccData.model.refreshShutdownRepetition()
                    }

                    onAccept: {
                        if (dccData.model.shutdownRepetition === 3) {
                            return
                        }
                        dccData.worker.setShutdownRepetition(3)
                    }
                }
            }
        }
        DccObject {
            id: repeatDaysEditObject
            property bool forceShow: false
            name: "repeatDaysEdit"
            parentName: "power/general/shutdownGroup"
            visible: (dccData.model.scheduledShutdownState && dccData.model.shutdownRepetition === 3)
            weight: 4
            pageType: DccObject.Editor
            page: RowLayout {
                Label {
                    text: {
                        var str = ""
                        var days = dccData.model.customShutdownWeekDays
                        for (var i = 0; i < days.length; i++) {
                            str += selectDayDialog.dateStr[days[i] - 1] + ", "
                        }
                        if (str.length > 1) {
                            str = str.slice(0, -2)
                        }
                        return str;
                    }
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

                    ScheduledShutdownDialog {
                        id: selectDayDialog
                    }
                }
            }
        }
    }
}
