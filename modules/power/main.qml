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
    property var listModelComponent: DListModelComponent {}
    
    function reset() {
        dbus_power.buttonPower = "interactive"
        dbus_power.lidCloseACAction = "suspend"
        dbus_power.lidCloseBatteryAction = "suspend"
        dbus_power.lockEnabled = true
        dbus_power.currentProfile = "Default"
    }

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
            case 6: return 0
        }
    }

    function indexToLabel(idx) {
        switch (idx) {
            case 0: return "1m"; break
            case 1: return "5m"; break
            case 2: return "10m"; break
            case 3: return "15m"; break
            case 4: return "30m"; break
            case 5: return "1h"; break
            case 6: return "Never"
        }
    }

    function getBatteryPercentage() {
        if (dbus_power.batteryIsPresent) {
            return dbus_power.batteryPercentage + "%"
            } else {
                return ""
            }
        }

        Column {
            anchors.fill: parent

            Item {
                width: parent.width
                height: title.height

                PowerTitle {
                    id: title
                    text: dsTr("Power")
                    hint: root.getBatteryPercentage()
                    showHyphen: dbus_power.batteryIsPresent
                    breath: dbus_power.state == 1
                    /* | value | description                        | */
                    /* |     1 | battery is charging                | */
                    /* |     2 | battery is in use                  | */
                    /* |     3 | battery is not in use              | */
                    /* |     4 | battery is full, using AC directly | */
                }

                DTextButton {
                    text: dsTr("Reset")

                    onClicked: reset()

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
                content.sourceComponent: GridView{
                    id: power_button_view
                    width: parent.width
                    height: 30

                    cellWidth: width/3
                    cellHeight: 30

                    model: {
                        var model = listModelComponent.createObject(power_button_view, {})
                        model.append({
                         "item_label": dsTr("Shutdown"),
                         "item_value": "shutdown"
                         })
                        model.append({
                         "item_label": dsTr("Suspend"),
                         "item_value": "suspend"
                         })                    
                        model.append({
                         "item_label": dsTr("Ask"),
                         "item_value": "interactive"
                         })                    
                        return model
                    }

                    delegate: PropertyItem {
                        currentValue: dbus_power.buttonPower
                        onSelectAction: {
                            dbus_power.buttonPower = itemValue
                        }
                    }
                }
            }
            DSeparatorHorizontal{ visible: close_the_lid_rect.visible }
            DBaseExpand {
                id: close_the_lid_rect
                visible: dbus_power.lidIsPresent
                expanded: true
                header.sourceComponent: DBaseLine {
                    leftLoader.sourceComponent: DssH2 {
                        text: dsTr("When I close the lid")
                    }
                }
                content.sourceComponent: GridView{
                    id: close_the_lid_view
                    width: parent.width
                    height: 30

                    cellWidth: width/3
                    cellHeight: 30

                    model: {
                        var model = listModelComponent.createObject(close_the_lid_view, {})
                        model.append({
                         "item_label": dsTr("Shutdown"),
                         "item_value": "shutdown"
                         })
                        model.append({
                         "item_label": dsTr("Suspend"),
                         "item_value": "suspend"
                         })                    
                        model.append({
                         "item_label": dsTr("Nothing"),
                         "item_value": "nothing"
                         })                    
                        return model
                    }

                    delegate: PropertyItem {
                        currentValue: dbus_power.lidCloseACAction
                        onSelectAction: {
                            dbus_power.lidCloseACAction = itemValue
                            dbus_power.lidCloseBatteryAction = itemValue
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
                content.sourceComponent: GridView{
                    id: wake_require_password_view
                    width: parent.width
                    height: 30

                    cellWidth: width/2
                    cellHeight: 30

                    model: {
                        var model = listModelComponent.createObject(wake_require_password_view, {})
                        model.append({
                         "item_label": dsTr("Requires a password"),
                         "item_value": true
                         })
                        model.append({
                         "item_label": dsTr("Without a password"),
                         "item_value": false
                         })                    
                        return model
                    }

                    delegate: PropertyItem {
                        currentValue: dbus_power.lockEnabled
                        onSelectAction: {
                            dbus_power.lockEnabled = itemValue
                        }
                    }
                }
            }
            DSeparatorHorizontal{}


            DBaseLine{}
            DBaseExpand {
                id: power_plan_ac_rect
                expanded: true
                header.sourceComponent: DBaseLine {
                    leftLoader.sourceComponent: ImageTitle {
                        imageSource: "images/power_plan_ac.png"        
                        title: dsTr("Power plan")
                    }
                }
                content.sourceComponent: Column {
                    GridView{
                        id: power_plan
                        width: parent.width
                        height: 30 * 2

                        cellWidth: width/2
                        cellHeight: 30

                        model: {
                            var model = listModelComponent.createObject(power_plan, {})
                            model.append({
                               "item_label": dsTr("Balance"),
                               "item_value": "Default"
                               })
                            model.append({
                               "item_label": dsTr("Power saver"),
                               "item_value": "Powersave"
                               })                    
                            model.append({
                               "item_label": dsTr("High performance"),
                               "item_value": "Performance"
                               })                    
                            model.append({
                               "item_label": dsTr("Custom"),
                               "item_value": "Customize"
                               })                    
                            return model
                        }

                        delegate: PropertyItem {
                            currentValue: dbus_power.currentProfile
                            onSelectAction: {
                                print(itemValue)
                                dbus_power.currentProfile = itemValue
                            }
                        }
                    }

                    TitleSeparator {
                        id: trun_of_monitor_sep
                        title: dsTr("Turn off monitor")
                        width: parent.width
                    }

                    Item {
                        width: parent.width
                        height: 30
                        GridView{
                            id: turn_off_monitor_view
                            anchors.fill: parent
                            anchors.leftMargin: 20
                            anchors.rightMargin: 20

                            cellWidth: width/7
                            cellHeight: 30

                            model: {
                                var model = listModelComponent.createObject(turn_off_monitor_view, {})
                                for(var i=0; i<7; i++){
                                    model.append({
                                       "item_label": indexToLabel(i),
                                       "item_value": indexToTimeout(i)
                                       })
                                }
                                return model
                            }

                            delegate: PropertyItem {
                                currentValue: dbus_power.idleDelay
                                onSelectAction: {
                                    dbus_power.currentProfile = "Customize"
                                    dbus_power.idleDelay = itemValue
                                }
                            }
                        }
                    }

                    TitleSeparator {
                        id: suspend_sep
                        title: dsTr("Suspend")
                        width: parent.width
                    }
                    Item {
                        width: parent.width
                        height: 30
                        GridView{
                            id: suspend_view
                            anchors.fill: parent
                            anchors.leftMargin: 20
                            anchors.rightMargin: 20


                            cellWidth: width/7
                            cellHeight: 30
                            property int currentValue: dbus_power.sleepInactiveAcTimeout

                            model: {
                                var model = listModelComponent.createObject(suspend_view, {})
                                for(var i=0; i<7; i++){
                                    model.append({
                                     "item_label": indexToLabel(i),
                                     "item_value": indexToTimeout(i)
                                     })
                                }
                                return model
                            }

                            delegate: PropertyItem {
                                currentValue: dbus_power.sleepInactiveAcTimeout
                                onSelectAction: {
                                    dbus_power.currentProfile = "Customize"
                                    dbus_power.sleepInactiveAcTimeout = itemValue
                                    dbus_power.sleepInactiveBatteryTimeout = itemValue
                                }
                            }
                        }
                    }
                }
            }
            DSeparatorHorizontal{}


            DBaseLine{}
            DBaseExpand {
                id: power_plan_battery_rect
                expanded: true
                header.sourceComponent: DBaseLine {
                    leftLoader.sourceComponent: ImageTitle {
                        imageSource: "images/power_plan_battery.png"
                        title: dsTr("Power plan")
                    }
                }
                content.sourceComponent: Column {
                    GridView{
                        id: power_plan
                        width: parent.width
                        height: 30 * 2

                        cellWidth: width/2
                        cellHeight: 30

                        model: {
                            var model = listModelComponent.createObject(power_plan, {})
                            model.append({
                               "item_label": dsTr("Balance"),
                               "item_value": "Default"
                               })
                            model.append({
                               "item_label": dsTr("Power saver"),
                               "item_value": "Powersave"
                               })                    
                            model.append({
                               "item_label": dsTr("High performance"),
                               "item_value": "Performance"
                               })                    
                            model.append({
                               "item_label": dsTr("Custom"),
                               "item_value": "Customize"
                               })                    
                            return model
                        }

                        delegate: PropertyItem {
                            currentValue: dbus_power.currentProfile
                            onSelectAction: {
                                print(itemValue)
                                dbus_power.currentProfile = itemValue
                            }
                        }
                    }

                    TitleSeparator {
                        id: trun_of_monitor_sep
                        title: dsTr("Turn off monitor")
                        width: parent.width
                    }

                    Item {
                        width: parent.width
                        height: 30
                        GridView{
                            id: turn_off_monitor_view
                            anchors.fill: parent
                            anchors.leftMargin: 20
                            anchors.rightMargin: 20

                            cellWidth: width/7
                            cellHeight: 30

                            model: {
                                var model = listModelComponent.createObject(turn_off_monitor_view, {})
                                for(var i=0; i<7; i++){
                                    model.append({
                                       "item_label": indexToLabel(i),
                                       "item_value": indexToTimeout(i)
                                       })
                                }
                                return model
                            }

                            delegate: PropertyItem {
                                currentValue: dbus_power.idleDelay
                                onSelectAction: {
                                    dbus_power.currentProfile = "Customize"
                                    dbus_power.idleDelay = itemValue
                                }
                            }
                        }
                    }

                    TitleSeparator {
                        id: suspend_sep
                        title: dsTr("Suspend")
                        width: parent.width
                    }
                    Item {
                        width: parent.width
                        height: 30
                        GridView{
                            id: suspend_view
                            anchors.fill: parent
                            anchors.leftMargin: 20
                            anchors.rightMargin: 20


                            cellWidth: width/7
                            cellHeight: 30
                            property int currentValue: dbus_power.sleepInactiveAcTimeout

                            model: {
                                var model = listModelComponent.createObject(suspend_view, {})
                                for(var i=0; i<7; i++){
                                    model.append({
                                     "item_label": indexToLabel(i),
                                     "item_value": indexToTimeout(i)
                                     })
                                }
                                return model
                            }

                            delegate: PropertyItem {
                                currentValue: dbus_power.sleepInactiveAcTimeout
                                onSelectAction: {
                                    dbus_power.currentProfile = "Customize"
                                    dbus_power.sleepInactiveAcTimeout = itemValue
                                    dbus_power.sleepInactiveBatteryTimeout = itemValue
                                }
                            }
                        }
                    }
                }
            }
            DSeparatorHorizontal{}
        }
    }
