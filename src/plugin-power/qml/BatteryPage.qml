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
        name: "screenAndSuspendTitle"
        parentName: "power/onBattery"
        displayName: qsTr("Screen and Suspend")
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
        name: "turnOffTheMonitorAfterGroup"
        parentName: "power/onBattery"
        weight: 100
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "turnOffTheMonitorAfter"
            parentName: "power/onBattery/turnOffTheMonitorAfterGroup"
            displayName: qsTr("Turn off the monitor after")
            weight: 1
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

                CustomTipsSlider {
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
        page: DccGroupView {}

        DccObject {
            name: "lockScreenAfter"
            parentName: "power/onBattery/lockScreenAfterGroup"
            displayName: qsTr("Lock screen after")
            weight: 1
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
                CustomTipsSlider {
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
        page: DccGroupView {}

        DccObject {
            name: "computerSuspendsAfter"
            parentName: "power/onBattery/computerSuspendsAfterGroup"
            displayName: qsTr("Computer suspends after")
            weight: 1
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
                CustomTipsSlider {
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
                width: 100
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
                width: 100
                model: dccData.batteryPressModel
                currentIndex: model.indexOfKey(dccData.model.batteryPressPowerBtnAction)

                onCurrentIndexChanged: {
                    dccData.worker.setBatteryPressPowerBtnAction(model.keyOfIndex(currentIndex))
                }
            }
        }
    }

    DccObject {
        name: "lowBatteryTitle"
        parentName: "power/onBattery"
        displayName: qsTr("Low Battery")
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
                width: 100
                textRole: "text"
                flat: true
                currentIndex: dccData.indexByValueOnModel(model, dccData.model.lowPowerNotifyThreshold)
                model: ListModel {
                    ListElement { text: qsTr("Disbale"); value: 10 }
                    ListElement { text: "10%"; value: 10 }
                    ListElement { text: "15%"; value: 15 }
                    ListElement { text: "20%"; value: 20 }
                    ListElement { text: "25%"; value: 25 }
                }

                onCurrentIndexChanged: {
                    var selectedValue = model.get(currentIndex).value;
                    dccData.worker.setLowPowerNotifyThreshold(selectedValue)
                }
            }
        }
    }

    DccObject {
        name: "lowBatteryOperatorGroup"
        parentName: "power/onBattery"
        weight: 700
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "lowBatteryOperator"
            parentName: "power/onBattery/lowBatteryOperatorGroup"
            displayName: qsTr("Low battery operation")
            weight: 1
            pageType: DccObject.Editor
            page: D.ComboBox {
                width: 100
                model: [ "Auto sleep", qsTr("Auto suspend")]
                flat: true
            }
        }
        DccObject {
            name: "lowBatteryThreshold"
            parentName: "power/onBattery/lowBatteryOperatorGroup"
            displayName: qsTr("Low battery threshold")
            weight: 2
            pageType: DccObject.Editor
            page: D.ComboBox {
                textRole: "text"
                width: 100
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
            }
        }
    }

    DccObject {
        name: "batteryManagementTitle"
        parentName: "power/onBattery"
        displayName: qsTr("Battery Management")
        weight: 800
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
                    text: dccData.model.batteryCapacity
                }

            DccObject {
                name: "optimizeChargingTime"
                parentName: "power/onBattery/batteryManagementGroup"
                displayName: qsTr("Optimize battery charging")
                weight: 3
                pageType: DccObject.Editor
                page: D.Switch {

                }
            }
            DccObject {
                name: "maximumChargingCapacity"
                parentName: "power/onBattery/batteryManagementGroup"
                displayName: qsTr("Maximum charging capacity")
                description: qsTr("When the battery is not frequently used, the maximum charging capacity of the battery can be limited to slow down battery aging")
                weight: 4
                pageType: DccObject.Editor
                page: D.ComboBox {
                    width: 100
                    model: [ "90%", "85%", "80%" ]
                    flat: true
                }
            }
        }
    }
}
