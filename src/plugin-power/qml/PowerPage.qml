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
        parentName: "power/onPower"
        displayName: qsTr("Screen and Suspend")
        visible: dccData.platformName() !== "wayland"
        weight: 10
    }

    DccObject {
        name: "turnOffTheMonitorAfterGroup"
        parentName: "power/onPower"
        weight: 100
        pageType: DccObject.Item
        visible: dccData.platformName() !== "wayland"
        page: DccGroupView {}

        DccObject {
            name: "turnOffTheMonitorAfter"
            parentName: "power/onPower/turnOffTheMonitorAfterGroup"
            displayName: qsTr("Turn off the monitor after")
            weight: 1
            pageType: DccObject.Item
            page: ColumnLayout {
                Layout.fillHeight: true

                RowLayout {
                    Layout.leftMargin: 14
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
                    dataMap: dccData.model.linePowerScreenBlackDelayModel

                    property real leftTextWidth: 0
                    property real rightTextWidth: 0
                    property real dynamicLeftMargin: Math.max(16, leftTextWidth / 2 + 5)
                    property real dynamicRightMargin: Math.max(16, rightTextWidth / 2 + 5)

                    Layout.preferredHeight: 90
                    Layout.alignment: Qt.AlignCenter
                    Layout.leftMargin: dynamicLeftMargin
                    Layout.rightMargin: dynamicRightMargin
                    Layout.topMargin: 10
                    Layout.bottomMargin: 10
                    Layout.fillWidth: true

                    TextMetrics {
                        font: offMonitorSlider.slider.font
                        text: offMonitorSlider.dataMap[0].text
                        Component.onCompleted: {
                            offMonitorSlider.leftTextWidth = width
                        }
                    }
                    TextMetrics {
                        font: offMonitorSlider.slider.font
                        text: offMonitorSlider.dataMap[offMonitorSlider.dataMap.length - 1].text
                        Component.onCompleted: {
                            offMonitorSlider.rightTextWidth = width
                        }
                    }
                    slider.value: dccData.indexByValueOnMap(dataMap, dccData.model.screenBlackDelayOnPower)
                    slider.onValueChanged: {
                        dccData.worker.setScreenBlackDelayOnPower(dataMap[slider.value].value)
                    }
                }
            }
        }
    }

    DccObject {
        name: "lockScreenAfterGroup"
        parentName: "power/onPower"
        weight: 200
        pageType: DccObject.Item
        visible: dccData.platformName() !== "wayland"
        page: DccGroupView {}

        DccObject {
            name: "lockScreenAfter"
            parentName: "power/onPower/lockScreenAfterGroup"
            displayName: qsTr("Lock screen after")
            weight: 1
            pageType: DccObject.Item
            page: ColumnLayout {
                Layout.fillHeight: true
                
                RowLayout {
                    Layout.leftMargin: 14
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
                    dataMap: dccData.model.linePowerLockDelayModel

                    property real leftTextWidth: 0
                    property real rightTextWidth: 0
                    property real dynamicLeftMargin: Math.max(16, leftTextWidth / 2 + 5)
                    property real dynamicRightMargin: Math.max(16, rightTextWidth / 2 + 5)

                    Layout.preferredHeight: 90
                    Layout.alignment: Qt.AlignCenter
                    Layout.leftMargin: dynamicLeftMargin
                    Layout.rightMargin: dynamicRightMargin
                    Layout.topMargin: 10
                    Layout.bottomMargin: 10
                    Layout.fillWidth: true

                    TextMetrics {
                        font: lockScreenSlider.slider.font
                        text: lockScreenSlider.dataMap[0].text
                        Component.onCompleted: {
                            lockScreenSlider.leftTextWidth = width
                        }
                    }
                    TextMetrics {
                        font: lockScreenSlider.slider.font
                        text: lockScreenSlider.dataMap[lockScreenSlider.dataMap.length - 1].text
                        Component.onCompleted: {
                            lockScreenSlider.rightTextWidth = width
                        }
                    }
                    slider.value: dccData.indexByValueOnMap(dataMap, dccData.model.powerLockScreenDelay)
                    slider.onValueChanged: {
                        dccData.worker.setLockScreenDelayOnPower(dataMap[slider.value].value)
                    }
                }
            }
        }
    }

    DccObject {
        name: "computerSuspendsAfterGroup"
        parentName: "power/onPower"
        weight: 300
        pageType: DccObject.Item
        visible: dccData.platformName() !== "wayland" && !dccData.model.isVirtualEnvironment && dccData.model.canSuspend
        page: DccGroupView {}

        DccObject {
            name: "computerSuspendsAfter"
            parentName: "power/onPower/computerSuspendsAfterGroup"
            displayName: qsTr("Computer suspends after")
            weight: 1
            pageType: DccObject.Item
            page: ColumnLayout {
                Layout.fillHeight: true
                
                RowLayout {
                    Layout.leftMargin: 14
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
                    dataMap: dccData.model.linePowerSleepDelayModel

                    property real leftTextWidth: 0
                    property real rightTextWidth: 0
                    property real dynamicLeftMargin: Math.max(16, leftTextWidth / 2 + 5)
                    property real dynamicRightMargin: Math.max(16, rightTextWidth / 2 + 5)

                    Layout.preferredHeight: 90
                    Layout.alignment: Qt.AlignCenter
                    Layout.leftMargin: dynamicLeftMargin
                    Layout.rightMargin: dynamicRightMargin
                    Layout.topMargin: 10
                    Layout.bottomMargin: 10
                    Layout.fillWidth: true

                    TextMetrics {
                        font: suspendsSlider.slider.font
                        text: suspendsSlider.dataMap[0].text
                        Component.onCompleted: {
                            suspendsSlider.leftTextWidth = width
                        }
                    }
                    TextMetrics {
                        font: suspendsSlider.slider.font
                        text: suspendsSlider.dataMap[suspendsSlider.dataMap.length - 1].text
                        Component.onCompleted: {
                            suspendsSlider.rightTextWidth = width
                        }
                    }
                    slider.value: dccData.indexByValueOnMap(dataMap, dccData.model.sleepDelayOnPower)
                    slider.onValueChanged: {
                        dccData.worker.setSleepDelayOnPower(dataMap[slider.value].value)
                    }
                }
            }
        }
    }

    DccObject {
        name: "powerButtonGroup"
        parentName: "power/onPower"
        weight: 400
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "whenTheLidIsClosed"
            parentName: "power/onPower/powerButtonGroup"
            displayName: qsTr("When the lid is closed")
            visible: dccData.model.lidPresent
            weight: 1
            pageType: DccObject.Editor
            page: CustomComboBox {
                textRole: "text"
                enableRole: "enable"
                visibleRole: "visible"
                model: dccData.powerLidModel
                currentIndex: model.indexOfKey(dccData.model.linePowerLidClosedAction)

                onCurrentIndexChanged: {
                    dccData.worker.setLinePowerLidClosedAction(model.keyOfIndex(currentIndex))
                }
            }
        }
        DccObject {
            name: "whenThePowerButtonIsPressed"
            parentName: "power/onPower/powerButtonGroup"
            displayName: qsTr("When the power button is pressed")
            weight: 2
            pageType: DccObject.Editor
            visible: dccData.platformName() !== "wayland"
            page: CustomComboBox {
                textRole: "text"
                enableRole: "enable"
                visibleRole: "visible"
                model: dccData.powerPressModel
                currentIndex: model.indexOfKey(dccData.model.linePowerPressPowerBtnAction)

                onCurrentIndexChanged: {
                    dccData.worker.setLinePowerPressPowerBtnAction(model.keyOfIndex(currentIndex))
                }
            }
        }
    }
}
