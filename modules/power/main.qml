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

        Item {
            width: parent.width
            height: title.height

            DssTitle {
                id: title
                text: dsTr("Power")
            }

            DTextButton {
                text: dsTr("Reset")

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        DSeparatorHorizontal {}

        DBaseExpand {
            id: power_button_rect
            expanded: true
            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: dsTr("When I press the power button")
                }
            }
            content.sourceComponent: DMultipleSelectView {
                id: power_button_view
                rows: 1
                columns: 3

                width: parent.width
                height: rows * 30
                singleSelectionMode: true

                model: ListModel {}
                Component.onCompleted: {
                    model.append({"label": dsTr("Shut down"), "selected": dbus_power.buttonPower == "shutdown"})
                    model.append({"label": dsTr("Suspend"), "selected": dbus_power.buttonPower == "suspend"})
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
        DSeparatorHorizontal{}
        DBaseExpand {
            id: close_the_lid_rect
            expanded: true
            visible: dbus_system_info.isLaptop ? true : false
            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: dsTr("When I close the lid")
                }
            }
            content.sourceComponent: DMultipleSelectView {
                id: close_the_lid_view
                rows: 1
                columns: 3

                width: parent.width
                height: rows * 30
                singleSelectionMode: true

                model: ListModel {}
                Component.onCompleted: {
                    model.append({"label": dsTr("Shut down"), "selected": dbus_power.lidCloseBatteryAction == "shutdown"})
                    model.append({"label": dsTr("Suspend"), "selected": dbus_power.lidCloseBatteryAction == "suspend"})
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
        DSeparatorHorizontal{}
        DBaseExpand {
            id: wake_require_password_expand
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
                    model.append({"label": dsTr("Requires a password"), "selected": dbus_power.lockEnabled})
                    model.append({"label": dsTr("Without a password"), "selected": !dbus_power.lockEnabled})
                }

                onSelect: {
                    dbus_power.lockEnabled = (index == 0)
                }

                Connections {
                    target: dbus_power
                    onLockEnabledChanged: {
                        var target = dbus_power.lockEnabled ? 0 : 1
                        wake_require_password_view.selectItem(target)
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
            content.sourceComponent: DMultipleSelectView {
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
        DSeparatorHorizontal{}
        DBaseExpand {
            id: turn_off_monitor_rect
            expanded: true
            property string headerText: "- " + dsTr("Turn off monitor")

            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: turn_off_monitor_rect.headerText
                }
            }
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
                        label: "never"
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
                    }
                }
            }
        }
        DSeparatorHorizontal{}
        DBaseExpand {
            id: suspend_rect
            expanded: true
            property string headerText: "- " + dsTr("Suspend")

            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: suspend_rect.headerText
                }
            }
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
                        label: "never"
                        selected: false
                    }
                }

                Component.onCompleted: {
                    suspend_view.selectItem(timeoutToIndex(dbus_power.sleepInactiveAc))
                }

                onSelect: {
                    dbus_power.currentPlan = "customized"
                    dbus_power.sleepInactiveAc = indexToTimeout(index)
                    dbus_power.sleepInactiveBattery = indexToTimeout(index)
                }

                Connections {
                    target: dbus_power
                    onSleepDisplayBatteryChanged: {
                        suspend_view.selectItem(timeoutToIndex(dbus_power.sleepInactiveBattery))
                    }
                    onSleepDisplayAcChanged: {
                        suspend_view.selectItem(timeoutToIndex(dbus_power.sleepInactiveAc))
                    }
                }
            }
        }
        DSeparatorHorizontal{}
    }
}
