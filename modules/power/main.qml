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

    property var dbus_power: Power{
		onButtonPowerChanged: {
			print(buttonPower)
		}
    }
	property var dbus_system_info: SystemInfo {}
	
	function timeoutToIndex(timeout) {
		switch (timeout) {
			case 60 * 1: return 0; break
			case 60 * 2: return 1; break
			case 60 * 3: return 2; break
			case 60 * 5: return 3; break
			case 60 * 10: return 4; break
			case 60 * 30: return 5; break
			case 60 * 60: return 6; break
			default: return 7
		}
	}
	
	function indexToTimeout(idx) {
		switch (idx) {
			case 0: return 60 * 1; break
			case 1: return 60 * 2; break
			case 2: return 60 * 3; break
			case 3: return 60 * 5; break
			case 4: return 60 * 10; break
			case 5: return 60 * 30; break
			case 6: return 60 * 60; break
			case 7: return null
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
			
			property string headerText: dsTr("Turn off monitor")
			
            header.sourceComponent: DDownArrowHeader {
				id: turn_off_monitor_header
                text: turn_off_monitor_rect.headerText
				onClicked: turn_off_monitor_rect.expanded = active
            }
            content.sourceComponent: DRadioButton {
				id: turn_off_monitor_button
				
				buttonModel: ListModel {
					ListElement {buttonId: "1_m"; buttonLabel: "1m"}
					ListElement {buttonId: "2_m"; buttonLabel: "2m"}
					ListElement {buttonId: "3_m"; buttonLabel: "3m"}
					ListElement {buttonId: "5_m"; buttonLabel: "5m"}
					ListElement {buttonId: "10_m"; buttonLabel: "10m"}
					ListElement {buttonId: "30_m"; buttonLabel: "30m"}
					ListElement {buttonId: "1_h"; buttonLabel: "1h"}
					
					Component.onCompleted: {
						append({"buttonId": "never", "buttonLabel": dsTr("Never")})
					}
				}

				width: parent.width
				height: 22
				
				Component.onCompleted: {
					turn_off_monitor_rect.headerText = dsTr("Turn off monitor") + 
					                                   "<font color='black'>(" + 
													   buttonModel.get(timeoutToIndex(dbus_power.sleepDisplayAc)).buttonLabel +
													   ")</font>"
												   
					turn_off_monitor_button.selectItem(timeoutToIndex(dbus_power.sleepDisplayAc))
				}
				onItemSelected: {
					/* dbus_power.currentPlan = "custom" */					
					/* dbus_power.sleepDisplayAc = indexToTimeout(idx) */
					/* dbus_power.sleepDisplayBattery = indexToTimeout(idx) */
					
					turn_off_monitor_rect.headerText = dsTr("Turn off monitor") + 
					                                   "<font color='black'>(" + 
													   buttonModel.get(timeoutToIndex(dbus_power.sleepDisplayAc)).buttonLabel +
													   ")</font>"
				}
			}
        }		
		
        DBaseExpand {
            id: suspend_rect
			property string headerText: dsTr("Suspend")
			
            header.sourceComponent: DDownArrowHeader {
                text: suspend_rect.headerText
				onClicked: suspend_rect.expanded = active
            }
            content.sourceComponent: DRadioButton {
				id: suspend_button

				buttonModel: ListModel {
					ListElement {buttonId: "1_m"; buttonLabel: "1m"}
					ListElement {buttonId: "2_m"; buttonLabel: "2m"}
					ListElement {buttonId: "3_m"; buttonLabel: "3m"}
					ListElement {buttonId: "5_m"; buttonLabel: "5m"}
					ListElement {buttonId: "10_m"; buttonLabel: "10m"}
					ListElement {buttonId: "30_m"; buttonLabel: "30m"}
					ListElement {buttonId: "1_h"; buttonLabel: "1h"}
					
					Component.onCompleted: {
						append({"buttonId": "never", "buttonLabel": dsTr("Never")})
					}
				}
				
				width: parent.width
				height: 22
				
				Component.onCompleted: {
					suspend_rect.headerText = dsTr("Turn off monitor") +
					                          "<font color='black'>(" +
											  buttonModel.get(timeoutToIndex(dbus_power.sleepInactiveAcTimeout)).buttonLabel +
											  ")</font>"
					
					suspend_button.selectItem(timeoutToIndex(dbus_power.sleepInactiveAcTimeout))
				}
				
				onItemSelected: {
					/* dbus_power.sleepInactiveAcTimeout = indexToTimeout(idx) */
					/* dbus_power.sleepInactiveBatteryTimeout = indexToTimeout(idx) */
					/* dbus_power.currentPlan = "custom" */
					
					/* suspend_rect.headerText = dsTr("Turn off monitor") +  */
					/*                           "<font color='black'>(" +  */
					/* 						  buttonModel.get(timeoutToIndex(dbus_power.sleepInactiveAc)).buttonLabel + */
					/* 						  ")</font>" */
				}
			}
        }
		
        DBaseExpand {
            id: wake_require_password
            header.sourceComponent: DSwitchButtonHeader {
                text: dsTr("Require password when computer wakes")
				active: dbus_power.lockEnabled ? true : false
				
				onActiveChanged: {
					dbus_power.lockEnabled = active
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
