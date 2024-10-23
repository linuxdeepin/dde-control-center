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
        name: "powerPlansTitle"
        parentName: "power/general"
        displayName: qsTr("Power Plans")
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
        name: "powerPlans"
        parentName: "power/general"
        weight: 100
        pageType: DccObject.Item
        page: PowerPlansListview {
        }
    }

    DccObject {
        name: "powerSavingSettingsTitle"
        parentName: "power/general"
        displayName: qsTr("Power Saving Settings")
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
        name: "powerSavingSettingsGroup"
        parentName: "power/general"
        weight: 300
        pageType: DccObject.Item
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
            visible: dccData.model.haveBettary
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
        parentName: "power/general"
        weight: 400
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "autoPowerSavingOnBattery"
            parentName: "power/general/savePowerGroup"
            displayName: qsTr("Auto power saving on battery")
            weight: 1
            visible: dccData.model.haveBettary
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
            parentName: "power/general/savePowerGroup"
            displayName: qsTr("Auto reduce screen brightness during power saving")
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
    }

    DccObject {
        name: "wakeupSettingsTitle"
        parentName: "power/general"
        displayName: qsTr("Wakeup Settings")
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
            name: "passwordIsRequiredToWakeUpTheMonitor"
            parentName: "power/general/wakeupSettingsGroup"
            displayName: qsTr("Password is required to wake up the monitor")
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
        name: "shutdownSettingTitle"
        parentName: "power/general"
        displayName: qsTr("Shutdown Settings")
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
        name: "shutdownGroup"
        parentName: "power/general"
        weight: 800
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "timedPoweroff"
            parentName: "power/general/shutdownGroup"
            displayName: qsTr("Scheduled Shutdown")
            weight: 1
            pageType: DccObject.Editor
            page: D.Switch {
                id: timedShutdownSwitch
            }
        }

        DccObject {
            name: "poweroffTime"
            parentName: "power/general/shutdownGroup"
            visible: timedShutdownSwitch.checked
            displayName: qsTr("Time")
            weight: 2
            pageType: DccObject.Editor
            page: RowLayout {
                DccTimeRange {
                    Layout.preferredWidth: 100
                }
            }
        }

        DccObject {
            name: "repeatDays"
            parentName: "power/general/shutdownGroup"
            visible: timedShutdownSwitch.checked
            displayName: qsTr("Repeat")
            weight: 3
            pageType: DccObject.Editor
            page: D.ComboBox {
                width: 100
                model: [ qsTr("Once"), qsTr("Every day"), qsTr("Working days"), qsTr("Custom Time") ]
                flat: true
            }
        }
        DccObject {
            name: "repeatDaysEdit"
            parentName: "power/general/shutdownGroup"
            visible: timedShutdownSwitch.checked
            weight: 4
            pageType: DccObject.Editor
            page: RowLayout {
                Label {
                    text: "Monday, Tuesday, Wednesday, Thursday"
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
                                background: DccItemBackground {
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
                                text: qsTr("Cancel")
                                onClicked: {
                                    selectDayDialog.close()
                                }
                            }
                            D.Button {
                                text: qsTr("Save")
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
