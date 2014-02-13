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

<<<<<<< HEAD
        DssTitle {
            text: dsTr("Power") + "  (" + dbus_power.batteryPercentage + "%)"
=======
        Item {
            width: root.width
            height: title.height

            DssTitle {
                id: title
                text: dsTr("Power") + "  (" + dbus_power.batteryPercentage + "%)"
            }

            DTextButton {
                text: dsTr("Reset")

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }
>>>>>>> power front end
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
<<<<<<< HEAD
        DBaseLine{}
=======
        DBaseExpand {
            id: wake_require_password_rect
            expanded: true
            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: dsTr("Require password when computer wakes")
                }
            }

            content.sourceComponent: DMultipleSelectView {
                id: wake_require_password_view
                rows: 1
                columns: 2

                width: parent.width
                height: rows * 30
                singleSelectionMode: true

                model: ListModel {}
                Component.onCompleted: {
                    model.append({"label": dsTr("Requires a password"),
                                  "selected": dbus_power.lockEnabled})
                    model.append({"label": dsTr("Without a password"),
                                  "selected": !dbus_power.lockEnabled})
                }

                onSelect: {
                    dbus_power.lockEnabled = (index == 0)
                }

                Connections {
                    target: dbus_power
                    onLockEnabledChanged: {
                        if (dbus_power.lockEnabled) {
                            wake_require_password_view.selectItem(0)
                        } else {
                            wake_require_password_view.selectItem(1)
                        }
                    }
                }
            }
        }
        DSeparatorHorizontal{}
        DBaseExpand {}
>>>>>>> power front end
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
<<<<<<< HEAD
            property string headerText: "-" + dsTr("Turn off monitor")
=======
            property string headerText: "- " + dsTr("Turn off monitor")
>>>>>>> power front end

            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: turn_off_monitor_rect.headerText
                }
            }
<<<<<<< HEAD
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
=======
            content.sourceComponent: DMultipleSelectView {
                id: turn_off_monitor_view
                rows: 1
                columns: 7

                width: parent.width
                height: rows * 30
                singleSelectionMode: true

                model: ListModel {
                    ListElement {
                        label: "1m"
                        selected: false
                    }
                    ListElement {
                        label: "5m"
                        selected: false
                    }
                    ListElement {
                        label: "10m"
                        selected: false
                    }
                    ListElement {
                        label: "15m"
                        selected: false
                    }
                    ListElement {
                        label: "30m"
                        selected: false
                    }
                    ListElement {
                        label: "1h"
                        selected: false
                    }
                    ListElement {
                        label: "Never"
                        selected: false
                    }
                }
                Component.onCompleted: {
                    turn_off_monitor_view.selectItem(timeoutToIndex(dbus_power.sleepDisplayAc))
                }

                onSelect: {
                    dbus_power.currentPlan = "customized"
                    dbus_power.sleepDisplayAc = indexToTimeout(index)
                    dbus_power.sleepDisplayBattery = indexToTimeout(index)
                }

                Connections {
                    target: dbus_power
                    onSleepDisplayBatteryChanged: {
                        turn_off_monitor_view.selectItem(timeoutToIndex(dbus_power.sleepDisplayBattery))
                    }
                    onSleepDisplayAcChanged: {
                        turn_off_monitor_view.selectItem(timeoutToIndex(dbus_power.sleepDisplayAc))
>>>>>>> power front end
                    }
                }
            }            
        }
        DSeparatorHorizontal{}
        DBaseExpand {
            id: suspend_rect
            expanded: true
<<<<<<< HEAD
            property string headerText: "-" + dsTr("Suspend")
=======
            property string headerText: "- " + dsTr("Suspend")
>>>>>>> power front end

            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: suspend_rect.headerText
                }
            }
<<<<<<< HEAD
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
=======
            content.sourceComponent: DMultipleSelectView {
                id: suspend_view
                rows: 1
                columns: 7

                width: parent.width
                height: rows * 30
                singleSelectionMode: true

                model: ListModel {
                    ListElement {
                        label: "1m"
                        selected: false
                    }
                    ListElement {
                        label: "5m"
                        selected: false
                    }
                    ListElement {
                        label: "10m"
                        selected: false
                    }
                    ListElement {
                        label: "15m"
                        selected: false
                    }
                    ListElement {
                        label: "30m"
                        selected: false
                    }
                    ListElement {
                        label: "1h"
                        selected: false
                    }
                    ListElement {
                        label: "Never"
                        selected: false
                    }
                }
                Component.onCompleted: {
                    suspend_view.selectItem(timeoutToIndex(dbus_power.sleepinActiveAcTimeout))
                }

                onSelect: {
                    dbus_power.currentPlan = "customized"
                    dbus_power.sleepinActiveAcTimeout = indexToTimeout(index)
                    dbus_power.sleepInactiveBatteryTimeout = indexToTimeout(index)
                }

                Connections {
                    target: dbus_power
                    onSleepInactiveBatteryTimeoutChanged: {
                        suspend_view.selectItem(timeoutToIndex(dbus_power.sleepInactiveBatteryTimeout))
                    }
                    onSleepInactiveAcTimeoutChanged: {
                        suspend_view.selectItem(timeoutToIndex(dbus_power.sleepinActiveAcTimeout))
>>>>>>> power front end
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
