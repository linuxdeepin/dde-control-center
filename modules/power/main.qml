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
            text: dsTr("Power") + "  (" + dbus_power.batteryPercentage + "%)"
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
                height: 30 * 2

                DSelectView {
                    id: power_plan_view
                    width: parent.width - 15 * 2
                    horizontalPadding: 20
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
                            itemText: "Power Saver"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "high-performance"
                            itemText: "High Performance"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "customized"
                            itemText: "Custom"
                            itemWidth: 0
                        }
                    }

                    onItemSelected: {
                        print(itemId)
                        dbus_power.currentPlan = itemId
                    }
                    Connections {
                        target: dbus_power
                        onCurrentPlanChanged: {
                            /* There's a bug that the currentPlan broadcasted with the signal is always undefined */
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
            property string headerText: "-" + dsTr("Turn off monitor")

            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: turn_off_monitor_rect.headerText
                }
            }
            content.sourceComponent: Item {
                width: root.width
                height: 30

                DSelectView {
                    id: turn_off_monitor_view
                    width: parent.width - 15 * 2
                    selectedItems: [timeoutToIndex(dbus_power.sleepInactiveBatteryTimeout)]
                    anchors.centerIn: parent

                    model: ListModel {
                        ListElement {
                            itemId: "1_m"
                            itemText: "1m"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "5_m"
                            itemText: "5m"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "10_m"
                            itemText: "10m"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "15_m"
                            itemText: "15m"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "30_m"
                            itemText: "30m"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "1_h"
                            itemText: "1h"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "never"
                            itemText: "Never"
                            itemWidth: 0
                        }
                    }

                    onItemSelected: {
                        dbus_power.sleepDisplayAc = root.indexToTimeout(idx)
                        dbus_power.sleepDisplayBattery = root.indexToTimeout(idx)
                    }
                    Connections {
                        target: dbus_power
                        onSleepDisplayBatteryChanged: {
                            turn_off_monitor_view.selectItem(timeoutToIndex(dbus_power.sleepDisplayBattery))
                        }
                        onSleepDisplayAcChanged: {
                            turn_off_monitor_view.selectItem(timeoutToIndex(dbus_power.sleepDisplayAc))
                        }
                    }
                }
            }
        }
        DSeparatorHorizontal{}
        DBaseExpand {
            id: suspend_rect
            expanded: true
            property string headerText: "-" + dsTr("Suspend")

            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: suspend_rect.headerText
                }
            }
            content.sourceComponent: Item {
                width: root.width
                height: 30

                DSelectView {
                    id: suspend_view
                    width: parent.width - 15 * 2
                    selectedItems: [timeoutToIndex(dbus_power.sleepInactiveAcTimeout)]
                    anchors.centerIn: parent

                    model: ListModel {
                        ListElement {
                            itemId: "1_m"
                            itemText: "1m"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "5_m"
                            itemText: "5m"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "10_m"
                            itemText: "10m"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "15_m"
                            itemText: "15m"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "30_m"
                            itemText: "30m"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "1_h"
                            itemText: "1h"
                            itemWidth: 0
                        }
                        ListElement {
                            itemId: "never"
                            itemText: "Never"
                            itemWidth: 0
                        }
                    }

                    onItemSelected: {
                        dbus_power.sleepInActiveAcTimeout = root.indexToTimeout(idx)
                        dbus_power.sleepInactiveBatteryTimeout = root.indexToTimeout(idx)
                    }
                    Connections {
                        target: dbus_power
                        onSleepInactiveAcTimeoutChanged: {
                            turn_off_monitor_view.selectItem(timeoutToIndex(dbus_power.sleepInActiveAcTimeout))
                        }
                        onSleepInactiveBatteryTimeoutChanged: {
                            turn_off_monitor_view.selectItem(timeoutToIndex(dbus_power.sleepInactiveBatteryTimeout))
                        }
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
