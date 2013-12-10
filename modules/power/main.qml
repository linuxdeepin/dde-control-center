import QtQuick 2.1
import DBus.Com.Deepin.Daemon.Power 1.0
import DBus.Com.Deepin.Daemon.SystemInfo 1.0
import "../widgets"

Rectangle {
    id: root
    color: constants.bgColor
    width: 310
    height: 600

    property var constants: DConstants{}

    property var dbus_power: Power{}
    property var dbus_system_info: SystemInfo {}

    function timeoutToIndex(timeout) {
        switch (timeout) {
            case 60 * 1: return 0; break
            case 60 * 5: return 1; break
            case 60 * 10: return 2; break
            case 60 * 15: return 3; break
            case 60 * 30: return 4; break
            case 60 * 60: return 5; break
            default: return 6
        }
    }

    function indexToTimeout(idx) {
        switch (idx) {
            case 0: return 60 * 1; break
            case 1: return 60 * 5; break
            case 2: return 60 * 10; break
            case 3: return 60 * 15; break
            case 4: return 60 * 30; break
            case 5: return 60 * 60; break
            case 6: return null
        }
    }

    Column {
        anchors.fill: parent

        DBaseExpand {
            id: power_button_rect
            header.sourceComponent: DDownArrowHeader {
                text: dsTr("When I press the sleep button")
                onClicked: power_button_rect.expanded = active
            }
            content.sourceComponent: MultipleSelectView {
                id: power_button_view
                rows: 1
                columns: 3

                width: parent.width
                height: rows * 30
                singleSelectionMode: true

                model: ListModel {}
                Component.onCompleted: {
                    model.append({"label": dsTr("Shut down"), "selected": dbus_power.buttonPower == "shutdown"})
                    model.append({"label": dsTr("Suspent"), "selected": dbus_power.buttonPower == "suspend"})
                    model.append({"label": dsTr("None"), "selected": dbus_power.buttonPower == "nothing"})
                }
                onSelect: {
                    switch (index) {
                        case 0:
                        dbus_power.buttonPower = "shutdown"
                        break
                        case 1:
                        dbus_power.buttonPower = "suspend"
                        break
                        case 2:
                        dbus_power.buttonPower = "nothing"
                    }
                }

                Connections {
                    target: dbus_power
                    onButtonPowerChanged: {
                        switch (dbus_power.buttonPower) {
                            case "shutdown": power_button_view.selectItem(0); break
                            case "suspend": power_button_view.selectItem(1); break
                            case "nothing": power_button_view.selectItem(2); break
                        }
                    }
                }
            }
        }
        DBaseExpand {
            id: close_the_lid_rect
            visible: dbus_system_info.isLaptop ? true : false
            header.sourceComponent: DDownArrowHeader {
                text: dsTr("When I close the lid")
                onClicked: close_the_lid_rect.expanded = active
            }
            content.sourceComponent: MultipleSelectView {
                id: close_the_lid_view
                rows: 1
                columns: 3

                width: parent.width
                height: rows * 30
                singleSelectionMode: true

                model: ListModel {}
                Component.onCompleted: {
                    model.append({"label": dsTr("Shut down"), "selected": dbus_power.lidCloseBatteryAction == "shutdown"})
                    model.append({"label": dsTr("Suspent"), "selected": dbus_power.lidCloseBatteryAction == "suspend"})
                    model.append({"label": dsTr("None"), "selected": dbus_power.lidCloseBatteryAction == "nothing"})
                }

                onSelect: {
                    switch (index) {
                        case 0:
                        dbus_power.lidCloseBatteryAction = "shutdown"
                        break
                        case 1:
                        dbus_power.lidCloseBatteryAction = "suspend"
                        break
                        case 2:
                        dbus_power.lidCloseBatteryAction = "nothing"
                    }
                }

                Connections {
                    target: dbus_power
                    onLidCloseBatteryActionChanged: {
                        switch (dbus_power.lidCloseBatteryAction) {
                            case "shutdown": close_the_lid_view.selectItem(0); break
                            case "suspend": close_the_lid_view.selectItem(1); break
                            case "nothing": close_the_lid_view.selectItem(2); break
                        }
                    }
                }
            }
        }
        DBaseExpand {
            id: power_plan_rect
            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Power Plan")
                onClicked: power_plan_rect.expanded = active
            }
            content.sourceComponent: MultipleSelectView {
                id: power_plan_view
                rows: 2
                columns: 2
                singleSelectionMode: true

                width: parent.width
                height: rows * 30
                viewWidth: 100

                model: ListModel {}
                Component.onCompleted: {
                    model.append({"label": dsTr("Balance"), "selected": dbus_power.currentPlan == "balance"})
                    model.append({"label": dsTr("Power saver"), "selected": dbus_power.currentPlan == "saving"})
                    model.append({"label": dsTr("High performance"), "selected": dbus_power.currentPlan == "high-performance"})
                    model.append({"label": dsTr("Custom"), "selected": dbus_power.currentPlan == "customized"})
                }
                onSelect: {
                    switch (index) {
                        case 0:
                        dbus_power.currentPlan = "balance"
                        break
                        case 1:
                        dbus_power.currentPlan = "saving"
                        break
                        case 2:
                        dbus_power.currentPlan = "high-performance"
                        break
                        case 3:
                        dbus_power.currentPlan = "customized"
                    }
                }
                Connections {
                    target: dbus_power
                    onCurrentPlanChanged: {
                        switch (dbus_power.currentPlan) {
                            case "balance": power_plan_view.selectItem(0); break
                            case "saving": power_plan_view.selectItem(1); break
                            case "high-performance": power_plan_view.selectItem(2); break
                            case "customized": power_plan_view.selectItem(3); break
                        }
                    }
                }
            }
        }

        DBaseExpand {
            id: turn_off_monitor_rect

            property string headerText: dsTr("Turn off monitor")

            header.sourceComponent: DDownArrowHeader {
                id: turn_off_monitor_header
                text: turn_off_monitor_rect.headerText
                onClicked: turn_off_monitor_rect.expanded = active
            }
            content.sourceComponent: DRadioButton {
                id: turn_off_monitor_button
                width: parent.width
                height: 22

                buttonModel: [
                    {"buttonId": "1_m", "buttonLabel": "1m"},
                    {"buttonId": "5_m", "buttonLabel": "5m"},
                    {"buttonId": "10_m", "buttonLabel": "10m"},
                    {"buttonId": "15_m", "buttonLabel": "15m"},
                    {"buttonId": "30_m", "buttonLabel": "30m"},
                    {"buttonId": "1_h", "buttonLabel": "1h"},
                    {"buttonId": "never", "buttonLabel": "Never"}
                ]

                initializeIndex: timeoutToIndex(dbus_power.sleepDisplayAc)

                Component.onCompleted: {
                    turn_off_monitor_rect.headerText = dsTr("Turn off monitor") +
                    "<font color='black'>(" +
                    buttonModel[timeoutToIndex(dbus_power.sleepDisplayAc)].buttonLabel +
                    ")</font>"
                }

                onItemSelected: {
                    dbus_power.currentPlan = "customized"
                    dbus_power.sleepDisplayAc = indexToTimeout(idx)
                    dbus_power.sleepDisplayBattery = indexToTimeout(idx)

                    turn_off_monitor_rect.headerText = dsTr("Turn off monitor") +
                    "<font color='black'>(" +
                    buttonModel[timeoutToIndex(dbus_power.sleepDisplayAc)].buttonLabel +
                    ")</font>"
                }

                Connections {
                    target: dbus_power
                    onSleepDisplayBatteryChanged: {
                        print("sleepDisplayAc", dbus_power.sleepDisplayAc)
                        turn_off_monitor_button.selectItem(timeoutToIndex(dbus_power.sleepDisplayBattery))
                    }
                    onSleepDisplayAcChanged: {
                        print("sleepDisplayBattery", dbus_power.sleepDisplayBattery)
                        turn_off_monitor_button.selectItem(timeoutToIndex(dbus_power.sleepDisplayAc))
                    }
                }
            }
        }

        DBaseExpand {
            id: suspend_rect

            property string headerText: dsTr("Suspend")

            header.sourceComponent: DDownArrowHeader {
                id: suspend_header
                text: suspend_rect.headerText
                onClicked: suspend_rect.expanded = active
            }
            content.sourceComponent: DRadioButton {
                id: suspend_button
                width: parent.width
                height: 22

                buttonModel: [
                    {"buttonId": "1_m", "buttonLabel": "1m"},
                    {"buttonId": "5_m", "buttonLabel": "5m"},
                    {"buttonId": "10_m", "buttonLabel": "10m"},
                    {"buttonId": "15_m", "buttonLabel": "15m"},
                    {"buttonId": "30_m", "buttonLabel": "30m"},
                    {"buttonId": "1_h", "buttonLabel": "1h"},
                    {"buttonId": "never", "buttonLabel": "Never"}
                ]

                initializeIndex: timeoutToIndex(dbus_power.sleepInactiveAcTimeout)

                Component.onCompleted: {
                    suspend_rect.headerText = dsTr("Turn off monitor") +
                    "<font color='black'>(" +
                    buttonModel[timeoutToIndex(dbus_power.sleepInactiveAcTimeout)].buttonLabel +
                    ")</font>"
                }

                onItemSelected: {
                    dbus_power.currentPlan = "customized"
                    dbus_power.sleepInactiveAcTimeout = indexToTimeout(idx)
                    dbus_power.sleepInactiveBatteryTimeout = indexToTimeout(idx)

                    suspend_rect.headerText = dsTr("Turn off monitor") +
                    "<font color='black'>(" +
                    buttonModel[timeoutToIndex(dbus_power.sleepInactiveAcTimeout)].buttonLabel +
                    ")</font>"
                }

                Connections {
                    target: dbus_power
                    onSleepInactiveBatteryTimeoutChanged: {
                        print("sleepInactiveAcTimeout", dbus_power.sleepInactiveAcTimeout)
                        suspend_button.selectItem(timeoutToIndex(dbus_power.sleepInactiveBatteryTimeout))
                    }
                    onSleepInactiveAcTimeoutChanged: {
                        print("sleepInactiveBatteryTimeout", dbus_power.sleepInactiveBatteryTimeout)
                        suspend_button.selectItem(timeoutToIndex(dbus_power.sleepinActiveAcTimeout))
                    }
                }
            }
        }

        DBaseExpand {
            id: wake_require_password_rect
            header.sourceComponent: DSwitchButtonHeader {
				id: wake_require_password_header
                text: dsTr("Require password when computer wakes")
                active: dbus_power.lockEnabled

                onActiveChanged: {
                    dbus_power.lockEnabled = active
                }
            }
			
            Connections {
                 target: dbus_power
                onLockEnabledChanged: {
					wake_require_password_header.active = dbus_power.lockEnabled
                }
            }
        }

    /* DBaseExpand { */
    /*     id: tray_show_battery_state */
    /*     header.sourceComponent: DSwitchButtonHeader { */
    /*         text: dsTr("Always show icon in the tray") */
    /*     } */
    /* } */
    }
}
