import QtQuick 2.1
import DBus.Com.Deepin.Daemon.Power 1.0
import Deepin.Widgets 1.0

Rectangle {
    id: root
    color: constants.bgColor
    width: 310
    height: 600

    property var constants: DConstants{}
    property var dbus_power: Power{}

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

        DssTitle {
            text: dsTr("Power") + "(" + dbus_power.batteryPercentage + ")"
        }

        DSeparatorHorizontal {}

        DBaseExpand {
            id: power_button_rect
            expanded: true
            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: dsTr("When I press the sleep button")
                }
            }
            content.sourceComponent: Item {
                width: root.width
                height: 30

                DSelectView {
                    id: power_button_view
                    width: parent.width - 15 * 2
                    selectedItems: [dbus_power.buttonPower]
                    anchors.centerIn: parent

                    model: ListModel {
                        ListElement {
                            itemId: "shutdown"
                            itemText: "Shutdown"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "suspend"
                            itemText: "Suspend"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "nothing"
                            itemText: "None"
                            itemWidth: 0
                        }
                    }

                    onItemSelected: {
                        dbus_power.buttonPower = itemId
                    }
                    
                    Connections {
                        target: dbus_power
                        onButtonPowerChanged: {
                            power_button_view.select(dbus_power.buttonPower)
                        }
                    }
                }
            }            
        }
        DSeparatorHorizontal{}
        DBaseExpand {
            id: close_the_lid_rect
            expanded: true
            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: dsTr("When I close the lid")
                }
            }
            content.sourceComponent: Item {
                width: root.width
                height: 30

                DSelectView {
                    id: close_the_lid_view
                    width: parent.width - 15 * 2
                    selectedItems: [dbus_power.lidCloseBatteryAction]
                    anchors.centerIn: parent

                    model: ListModel {
                        ListElement {
                            itemId: "shutdown"
                            itemText: "Shutdown"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "suspend"
                            itemText: "Suspend"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "nothing"
                            itemText: "None"
                            itemWidth: 0
                        }
                    }

                    onItemSelected: {
                        dbus_power.lidCloseBatteryAction = itemId
                    }
                    
                    Connections {
                        target: dbus_power
                        onLidCloseBatteryActionChanged: {
                            close_the_lid_view.select(dbus_power.lidCloseBatteryAction)
                        }
                    }
                }
            }
        }
        DSeparatorHorizontal{}
        DBaseExpand {
            id: wake_require_password_rect
            expanded: true
            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: dsTr("Require password when computer wakes")
                }
            }
            content.sourceComponent: Item {
                width: root.width
                height: 30

                DSelectView {
                    width: parent.width - 15 * 2
                    selectedItems: ["require_yes"]
                    anchors.centerIn: parent

                    model: ListModel {
                        ListElement {
                            itemId: "require_yes"
                            itemText: "Requires a password"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "require_no"
                            itemText: "Without a password"
                            itemWidth: 0
                        }
                    }
                }
            }
        }
        DSeparatorHorizontal{}
        DBaseLine{}
        DBaseExpand {
            id: power_plan_rect
            expanded: true
            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: dsTr("Power plan")
                }
            }
            content.sourceComponent: Item {
                width: root.width
                height: 30

                DSelectView {
                    id: power_plan_view
                    width: parent.width - 15 * 2
                    selectedItems: [dbus_power.lidCloseBatteryAction]
                    anchors.centerIn: parent

                    model: ListModel {
                        ListElement {
                            itemId: "balance"
                            itemText: "Balance"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "saving"
                            itemText: "Saving"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "High Performance"
                            itemText: "high-performance"
                            itemWidth: 0
                        }                        
                        ListElement {
                            itemId: "customized"
                            itemText: "Custom"
                            itemWidth: 0
                        }
                    }

                    onItemSelected: {
                        dbus_power.currentPlan = itemId
                    }
                    
                    Connections {
                        target: dbus_power
                        onCurrentPlanChanged: {
                            power_plan_view.select(dbus_power.currentPlan)
                        }
                    }
                }
            }            
        }
        DSeparatorHorizontal{}
        DBaseExpand {
            id: turn_off_monitor_rect
            expanded: true
            property string headerText: dsTr("Turn off monitor")

            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: turn_off_monitor_rect.headerText
                }
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
        DSeparatorHorizontal{}
        DBaseExpand {
            id: suspend_rect
            expanded: true
            property string headerText: dsTr("Suspend")

            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: suspend_rect.headerText
                }
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

        DSeparatorHorizontal{}

    /* DBaseExpand { */
    /*     id: tray_show_battery_state */
    /*     header.sourceComponent: DSwitchButtonHeader { */
    /*         text: dsTr("Always show icon in the tray") */
    /*     } */
    /* } */
    }
}
