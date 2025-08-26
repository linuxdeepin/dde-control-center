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
        name: "screenAndSuspendTitle"
        parentName: "power/onBattery"
        displayName: qsTr("Screen and Suspend")
        visible: dccData.platformName() !== "wayland"
        weight: 10
    }

    DccObject {
        name: "turnOffTheMonitorAfterGroup"
        parentName: "power/onBattery"
        weight: 100
        pageType: DccObject.Item
        visible: dccData.platformName() !== "wayland"
        page: DccGroupView {}

        DccObject {
            name: "turnOffTheMonitorAfter"
            parentName: "power/onBattery/turnOffTheMonitorAfterGroup"
            displayName: qsTr("Turn off the monitor after")
            weight: 1
            pageType: DccObject.Item
            page: ColumnLayout {
                Layout.fillHeight: true

                RowLayout {
                    Layout.leftMargin: 10
                    Layout.rightMargin: 10
                    Layout.topMargin: 10
                    Label {
                        font: D.DTK.fontManager.t6
                        text: dccObj.displayName
                    }
                    Item {
                        Layout.fillWidth: true
                    }
                    Label {
                        text: offMonitorSlider.dataMap[offMonitorSlider.slider.value].trText
                        horizontalAlignment: Text.AlignRight
                    }
                }

                CustomTipsSlider {
                    id: offMonitorSlider
                    dataMap: dccData.model.batteryScreenBlackDelayModel
                    Layout.preferredHeight: 80
                    Layout.alignment: Qt.AlignCenter
                    Layout.margins: 10
                    Layout.fillWidth: true
                    slider.value: dccData.indexByValueOnMap(dataMap, dccData.model.screenBlackDelayOnBattery)
                    slider.onValueChanged: {
                        dccData.worker.setScreenBlackDelayOnBattery(dataMap[slider.value].value)
                    }
                }
            }
        }
    }

    DccObject {
        name: "lockScreenAfterGroup"
        parentName: "power/onBattery"
        weight: 200
        pageType: DccObject.Item
        visible: dccData.platformName() !== "wayland"
        page: DccGroupView {}

        DccObject {
            name: "lockScreenAfter"
            parentName: "power/onBattery/lockScreenAfterGroup"
            displayName: qsTr("Lock screen after")
            weight: 1
            pageType: DccObject.Item
            page: ColumnLayout {
                Layout.fillHeight: true

                RowLayout {
                    Layout.leftMargin: 10
                    Layout.rightMargin: 10
                    Layout.topMargin: 10
                    Label {
                        font: D.DTK.fontManager.t6
                        text: dccObj.displayName
                    }
                    Item {
                        Layout.fillWidth: true
                    }
                    Label {
                        text: lockScreenSlider.dataMap[lockScreenSlider.slider.value].trText
                        horizontalAlignment: Text.AlignRight
                    }
                }

                CustomTipsSlider {
                    id: lockScreenSlider
                    dataMap: dccData.model.batteryLockDelayModel
                    Layout.preferredHeight: 80
                    Layout.alignment: Qt.AlignCenter
                    Layout.margins: 10
                    Layout.fillWidth: true
                    slider.value: dccData.indexByValueOnMap(dataMap, dccData.model.batteryLockScreenDelay)
                    slider.onValueChanged: {
                        dccData.worker.setLockScreenDelayOnBattery(dataMap[slider.value].value)
                    }
                }
            }
        }
    }

    DccObject {
        name: "computerSuspendsAfterGroup"
        parentName: "power/onBattery"
        weight: 300
        pageType: DccObject.Item
        visible: dccData.platformName() !== "wayland" && !dccData.model.isVirtualEnvironment && dccData.model.canSuspend
        page: DccGroupView {}

        DccObject {
            name: "computerSuspendsAfter"
            parentName: "power/onBattery/computerSuspendsAfterGroup"
            displayName: qsTr("Computer suspends after")
            weight: 1
            pageType: DccObject.Item
            page: ColumnLayout {
                Layout.fillHeight: true

                RowLayout {
                    Layout.leftMargin: 10
                    Layout.rightMargin: 10
                    Layout.topMargin: 10
                    Label {
                        font: D.DTK.fontManager.t6
                        text: dccObj.displayName
                    }
                    Item {
                        Layout.fillWidth: true
                    }
                    Label {
                        text: suspendsSlider.dataMap[suspendsSlider.slider.value].trText
                        horizontalAlignment: Text.AlignRight
                    }
                }

                CustomTipsSlider {
                    id: suspendsSlider
                    dataMap: dccData.model.batterySleepDelayModel
                    Layout.preferredHeight: 80
                    Layout.alignment: Qt.AlignCenter
                    Layout.margins: 10
                    Layout.fillWidth: true
                    slider.value: dccData.indexByValueOnMap(dataMap, dccData.model.sleepDelayOnBattery)
                    slider.onValueChanged: {
                        dccData.worker.setSleepDelayOnBattery(dataMap[slider.value].value)
                    }
                }
            }
        }
    }

    DccObject {
        name: "powerButtonGroup"
        parentName: "power/onBattery"
        weight: 400
        pageType: DccObject.Item
        visible: dccData.platformName() !== "wayland"
        page: DccGroupView {}

        DccObject {
            name: "whenTheLidIsClosed"
            parentName: "power/onBattery/powerButtonGroup"
            displayName: qsTr("When the lid is closed")
            visible: dccData.model.lidPresent
            weight: 1
            pageType: DccObject.Editor
            page: CustomComboBox {
                textRole: "text"
                enableRole: "enable"
                visibleRole: "visible"
                model: dccData.batteryLidModel
                currentIndex: model.indexOfKey(dccData.model.batteryLidClosedAction)

                onCurrentIndexChanged: {
                    dccData.worker.setBatteryLidClosedAction(model.keyOfIndex(currentIndex))
                }
            }
        }
        DccObject {
            name: "whenThePowerButtonIsPressed"
            parentName: "power/onBattery/powerButtonGroup"
            displayName: qsTr("When the power button is pressed")
            weight: 2
            pageType: DccObject.Editor
            page: CustomComboBox {
                textRole: "text"
                enableRole: "enable"
                visibleRole: "visible"
                model: dccData.batteryPressModel
                currentIndex: model.indexOfKey(dccData.model.batteryPressPowerBtnAction)

                onCurrentIndexChanged: {
                    dccData.worker.setBatteryPressPowerBtnAction(model.keyOfIndex(currentIndex))
                }
            }
        }
    }

    DccTitleObject {
        name: "lowBatteryTitle"
        parentName: "power/onBattery"
        displayName: qsTr("Low Battery")
        weight: 500
    }

    DccObject {
        name: "lowBatteryNotificationGroup"
        parentName: "power/onBattery"
        weight: 600
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "lowBatteryNotification"
            parentName: "power/onBattery/lowBatteryNotificationGroup"
            displayName: qsTr("Low battery notification")
            weight: 1
            pageType: DccObject.Editor
            page: D.ComboBox {
                textRole: "text"
                flat: true
                currentIndex: dccData.indexByValueOnModel(model, dccData.model.lowPowerNotifyThreshold)
                model: ListModel {
                    ListElement { text: qsTr("Disable"); value: 0 }
                    ListElement { text: "10%"; value: 10 }
                    ListElement { text: "15%"; value: 15 }
                    ListElement { text: "20%"; value: 20 }
                    ListElement { text: "25%"; value: 25 }
                }

                onCurrentIndexChanged: {
                    var selectedValue = model.get(currentIndex).value;
                    if (selectedValue === 0) {
                        dccData.worker.setLowPowerNotifyEnable(false)
                        dccData.worker.setLowPowerNotifyThreshold(selectedValue)
                    } else {
                        dccData.worker.setLowPowerNotifyEnable(true)
                        dccData.worker.setLowPowerNotifyThreshold(selectedValue)
                    }
                }
            }
        }
    }

    DccObject {
        name: "lowBatteryOperatorGroup"
        parentName: "power/onBattery"
        weight: 700
        pageType: DccObject.Item
        visible: dccData.platformName() !== "wayland" && (dccData.model.canSuspend || dccData.model.canHibernate)
        page: DccGroupView {}

        DccObject {
            name: "lowBatteryOperator"
            parentName: "power/onBattery/lowBatteryOperatorGroup"
            displayName: qsTr("Low battery level")
            weight: 1
            pageType: DccObject.Editor
            page: CustomComboBox {
                textRole: "text"
                valueRole: "value"
                flat: true
                currentIndex: indexByValue(dccData.model.lowPowerAction)
                model: ListModel {
                    ListElement { text: qsTr("Auto suspend"); value: 0 }
                    ListElement { text: qsTr("Auto Hibernate"); value: 1 }
                }
                onCurrentIndexChanged: {
                    var selectedValue = model.get(currentIndex).value;
                    dccData.worker.setLowPowerAction(selectedValue)
                }
            }
        }
        DccObject {
            name: "lowBatteryThreshold"
            parentName: "power/onBattery/lowBatteryOperatorGroup"
            displayName: qsTr("Low battery threshold")
            weight: 2
            pageType: DccObject.Editor
            page: CustomComboBox {
                textRole: "text"
                valueRole: "value"
                currentIndex: indexByValue(dccData.model.lowPowerAutoSleepThreshold)
                model: ListModel {
                    ListElement { text: "1%"; value: 1 }
                    ListElement { text: "2%"; value: 2 }
                    ListElement { text: "3%"; value: 3 }
                    ListElement { text: "4%"; value: 4 }
                    ListElement { text: "5%"; value: 5 }
                    ListElement { text: "6%"; value: 6 }
                    ListElement { text: "7%"; value: 7 }
                    ListElement { text: "8%"; value: 8 }
                    ListElement { text: "9%"; value: 9 }
                }
                flat: true

                onCurrentIndexChanged: {
                    var selectedValue = model.get(currentIndex).value;
                    dccData.worker.setLowPowerAutoSleepThreshold(selectedValue)
                }
            }
        }
    }

    DccTitleObject {
        name: "batteryManagementTitle"
        parentName: "power/onBattery"
        displayName: qsTr("Battery Management")
        weight: 800
    }

    DccObject {
        name: "batteryManagementGroup"
        parentName: "power/onBattery"
        weight: 900
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "displayRemainingUsingAndChargingTime"
            parentName: "power/onBattery/batteryManagementGroup"
            displayName: qsTr("Display remaining using and charging time")
            weight: 1
            pageType: DccObject.Editor
            page: D.Switch {
                checked: dccData.model.showBatteryTimeToFull
                onCheckedChanged: {
                    dccData.worker.setShowBatteryTimeToFull(checked)
                }
            }
        }
        DccObject {
            name: "maximumCapacity"
            parentName: "power/onBattery/batteryManagementGroup"
            displayName: qsTr("Maximum capacity")
            weight: 2
            pageType: DccObject.Editor
            page: Label {
                    Layout.leftMargin: 10
                    text: dccData.model.batteryCapacity + "%"
                }
        }
    }
}
