import QtQuick 2.1
import DBus.Com.Deepin.Daemon.Power 1.0
import "../widgets"

Rectangle {
    id: root
    color: constants.bgColor
    width: 310
    height: 600
	
	property var constants: DConstants{}

    property var dbus_power: Power{
		Component.onCompleted: {
			print(suspendTime)
		}
    }
	
    Column {
        anchors.fill: parent

        DBaseExpand {
            id: press_sleep_button_rect
            header.sourceComponent: DDownArrowHeader {
                text: dsTr("When I press the sleep button")
				onClicked: press_sleep_button_rect.expanded = active
            }
            content.sourceComponent: MultipleSelectView {
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
            }
        }
        DBaseExpand {
            id: close_the_lid_rect
            header.sourceComponent: DDownArrowHeader {
                text: dsTr("When I close the lid")
				onClicked: close_the_lid_rect.expanded = active
            }
            content.sourceComponent: MultipleSelectView {
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
            }
        }
        DBaseExpand {
            id: power_plan_rect
            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Power Plan")
				onClicked: power_plan_rect.expanded = active
            }
            content.sourceComponent: MultipleSelectView {
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
            }
        }		
		
        DBaseExpand {
            id: turn_off_monitor_rect
            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Turn off monitor")
				onClicked: turn_off_monitor_rect.expanded = active
            }
            content.sourceComponent: DRadioButton {
				buttonModels: [{"buttonId": "1_m", "buttonLabel": "1m"},
							   {"buttonId": "2_m", "buttonLabel": "2m"},
							   {"buttonId": "3_m", "buttonLabel": "3m"},
							   {"buttonId": "5_m", "buttonLabel": "5m"},
							   {"buttonId": "10_m", "buttonLabel": "10m"},
							   {"buttonId": "30_m", "buttonLabel": "30m"},
							   {"buttonId": "1_h", "buttonLabel": "1h"},							   
							   {"buttonId": "never", "buttonLabel": dsTr("Never")}]
				width: parent.width
				height: 22
			}
        }		
		
        DBaseExpand {
            id: suspend_rect
            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Suspend")
				onClicked: suspend_rect.expanded = active
            }
            content.sourceComponent: DRadioButton {
				buttonModels: [{"buttonId": "1_m", "buttonLabel": "1m"},
							   {"buttonId": "2_m", "buttonLabel": "2m"},
							   {"buttonId": "3_m", "buttonLabel": "3m"},
							   {"buttonId": "5_m", "buttonLabel": "5m"},
							   {"buttonId": "10_m", "buttonLabel": "10m"},
							   {"buttonId": "30_m", "buttonLabel": "30m"},
							   {"buttonId": "1_h", "buttonLabel": "1h"},							   
							   {"buttonId": "never", "buttonLabel": dsTr("Never")}]
				width: parent.width
				height: 22
			}
        }		
		
        DBaseExpand {
            id: wake_require_password
            header.sourceComponent: DSwitchButtonHeader {
                text: dsTr("Require password when computer wakes")
            }
        }
        DBaseExpand {
            id: tray_show_battery_state
            header.sourceComponent: DSwitchButtonHeader {
                text: dsTr("Always show icon in the tray")
            }
        }
    }
}
