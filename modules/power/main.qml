import QtQuick 2.1
import DBus.Com.Deepin.Daemon.Power 1.0
import Deepin.Widgets 1.0
import "../shared"

Rectangle {
    id: power
    color: constants.bgColor
    width: 310
    height: 600

    property var constants: DConstants{}
    property var dbus_power: Power{}
    property var listModelComponent: DListModelComponent {}
    property var planInfo: JSON.parse(dbus_power.planInfo)

    function formatTime(timeout) {
        return indexToText(timeoutToIndex(timeout))
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
            case 6: return dsTr("Never")
        }
    }

    function indexToText(idx) {
        switch (idx) {
            case 0: return dsTr("1 minute"); break
            case 1: return dsTr("5 minutes"); break
            case 2: return dsTr("10 minutes"); break
            case 3: return dsTr("15 minutes"); break
            case 4: return dsTr("30 minutes"); break
            case 5: return dsTr("1 hour"); break
            case 6: return dsTr("Never")
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
        id: title_column
        width: power.width
        z: 3

        Item {
            width: power.width
            height: title.height

            PowerTitle {
                id: title
                text: modulesId.moduleLocaleNames["power"]
                hint: power.getBatteryPercentage()
                showHyphen: dbus_power.batteryIsPresent
                breath: dbus_power.batteryState == 1
            }

            ResetButton {
                onClicked: dbus_power.Reset()

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        
        DSeparatorHorizontal {}
    }

    DFlickable {
        anchors.top: title_column.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        contentWidth: main_column.childrenRect.width
        contentHeight: main_column.childrenRect.height

        Column {
            id: main_column
            width: power.width

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
                                         "item_value": 2,
                                         "item_tooltip": ""
                                     })
                        model.append({
                                         "item_label": dsTr("Suspend"),
                                         "item_value": 1,
                                         "item_tooltip": ""
                                     })
                        model.append({
                                         "item_label": dsTr("Ask"),
                                         "item_value": 4,
                                         "item_tooltip": ""
                                     })
                        return model
                    }

                    delegate: PropertyItem {
                        currentValue: dbus_power.powerButtonAction
                        onSelectAction: {
                            dbus_power.powerButtonAction = itemValue
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
                                         "item_value": 2,
                                         "item_tooltip": ""
                                     })
                        model.append({
                                         "item_label": dsTr("Suspend"),
                                         "item_value": 1,
                                         "item_tooltip": ""
                                     })
                        model.append({
                                         "item_label": dsTr("Nothing"),
                                         "item_value": 0,
                                         "item_tooltip": ""
                                     })
                        return model
                    }

                    delegate: PropertyItem {
                        currentValue: dbus_power.lidClosedAction
                        onSelectAction: {
                            dbus_power.lidClosedAction = itemValue
                        }
                    }
                }
            }
            DSeparatorHorizontal{}
            DBaseLine {
                id: wake_require_password_expand
                leftLoader.sourceComponent: DssH2 {
                    text: dsTr("Password required to wake up")
                }

                rightLoader.sourceComponent: Component{
                    DSwitchButton {
                        checked: dbus_power.lockWhenActive

                        onClicked: {
                            dbus_power.lockWhenActive = checked
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
                    leftLoader.sourceComponent: DssH2 {
                        text: dsTr("Plugged in")
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
                                             "item_label": dsTr("Balanced"),
                                             "item_value": 2,
                                             "item_tooltip": dsTr("Turn off the display") + ": " + formatTime(planInfo.PowerLine.Balanced[0]) + "\n" +
                                             dsTr("Suspend") + ": " + formatTime(planInfo.PowerLine.Balanced[1])
                                         })
                            model.append({
                                             "item_label": dsTr("Power saver"),
                                             "item_value": 1,
                                             "item_tooltip": dsTr("Turn off the display") + ": " + formatTime(planInfo.PowerLine.PowerSaver[0]) + "\n" +
                                             dsTr("Suspend") + ": " + formatTime(planInfo.PowerLine.PowerSaver[1])
                                         })
                            model.append({
                                             "item_label": dsTr("High performance"),
                                             "item_value": 3,
                                             "item_tooltip": dsTr("Turn off the display") + ": " + formatTime(planInfo.PowerLine.HighPerformance[0]) + "\n" +
                                             dsTr("Suspend") + ": " + formatTime(planInfo.PowerLine.HighPerformance[1])
                                         })
                            model.append({
                                             "item_label": dsTr("Custom"),
                                             "item_value": 0,
                                             "item_tooltip": dsTr("Turn off the display") + ": " + formatTime(planInfo.PowerLine.Custom[0]) + "\n" +
                                             dsTr("Suspend") + ": " + formatTime(planInfo.PowerLine.Custom[1])
                                         })
                            return model
                        }

                        delegate: PropertyItem {
                            currentValue: dbus_power.linePowerPlan
                            onSelectAction: {
                                dbus_power.linePowerPlan = itemValue
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: dbus_power.linePowerPlan == 0 ? ac_custom_column.height + arrow_rect_1.arrowHeight :  0
                visible: dbus_power.linePowerPlan == 0
                color: dconstants.contentBgColor

                Behavior on height {
                    SmoothedAnimation { duration: 300 }
                }

                ArrowRect {
                    id: arrow_rect_1
                    fillStyle: dconstants.bgColor
                    stroke: false
                    radius: 0
                    lineWidth: 0
                    arrowPosition: 0.75

                    anchors.fill: parent
                }
                Column {
                    id: ac_custom_column
                    width: parent.width
                    anchors.top: parent.top
                    anchors.topMargin: arrow_rect_1.arrowHeight

                    TitleSeparator {
                        text: dsTr("Custom Options")
                    }

                    TitleSeparator {
                        width: parent.width
                        text: dsTr("Turn off the display")
                        color: dconstants.contentBgColor
                    }

                    Rectangle {
                        width: parent.width
                        height: 30
                        color: dconstants.contentBgColor
                        GridView{
                            id: ac_turn_off_monitor_view
                            anchors.fill: parent
                            anchors.leftMargin: 20
                            anchors.rightMargin: 20

                            cellWidth: width/7
                            cellHeight: 30

                            model: {
                                var model = listModelComponent.createObject(ac_turn_off_monitor_view, {})
                                for(var i=0; i<7; i++){
                                    model.append({
                                                     "item_label": indexToLabel(i),
                                                     "item_value": indexToTimeout(i),
                                                     "item_tooltip": ""
                                                 })
                                }
                                return model
                            }

                            delegate: PropertyItem {
                                currentValue: dbus_power.linePowerIdleDelay
                                onSelectAction: {
                                    dbus_power.linePowerPlan = 0
                                    dbus_power.linePowerIdleDelay = itemValue
                                }
                            }
                        }
                    }
                    TitleSeparator {
                        width: parent.width
                        text: dsTr("Suspend")
                        color: dconstants.contentBgColor
                    }
                    Rectangle {
                        width: parent.width
                        height: 30
                        color: dconstants.contentBgColor
                        GridView{
                            id: ac_suspend_view
                            anchors.fill: parent
                            anchors.leftMargin: 20
                            anchors.rightMargin: 20


                            cellWidth: width/7
                            cellHeight: 30

                            model: {
                                var model = listModelComponent.createObject(ac_suspend_view, {})
                                for(var i=0; i<7; i++){
                                    model.append({
                                                     "item_label": indexToLabel(i),
                                                     "item_value": indexToTimeout(i),
                                                     "item_tooltip": ""
                                                 })
                                }
                                return model
                            }

                            delegate: PropertyItem {
                                currentValue: dbus_power.linePowerSuspendDelay
                                onSelectAction: {
                                    dbus_power.linePowerPlan = 0
                                    dbus_power.linePowerSuspendDelay = itemValue
                                }
                            }
                        }
                    }
                }
            }
            DSeparatorHorizontal{
                visible: power_plan_battery_rect.visible
            }


            DBaseLine{visible: power_plan_battery_rect.visible}
            DBaseExpand {
                id: power_plan_battery_rect
                visible: dbus_power.batteryIsPresent
                expanded: true
                header.sourceComponent: DBaseLine {
                    leftLoader.sourceComponent: DssH2 {
                        text: dsTr("On battery")
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
                                             "item_label": dsTr("Balanced"),
                                             "item_value": 2,
                                             "item_tooltip": dsTr("Turn off the display") + ": " + formatTime(planInfo.Battery.Balanced[0]) + "\n" +
                                             dsTr("Suspend") + ": " + formatTime(planInfo.Battery.Balanced[1])
                                         })
                            model.append({
                                             "item_label": dsTr("Power saver"),
                                             "item_value": 1,
                                             "item_tooltip": dsTr("Turn off the display") + ": " + formatTime(planInfo.Battery.PowerSaver[0]) + "\n" +
                                             dsTr("Suspend") + ": " + formatTime(planInfo.Battery.PowerSaver[1])
                                         })
                            model.append({
                                             "item_label": dsTr("High performance"),
                                             "item_value": 3,
                                             "item_tooltip": dsTr("Turn off the display") + ": " + formatTime(planInfo.Battery.HighPerformance[0]) + "\n" +
                                             dsTr("Suspend") + ": " + formatTime(planInfo.Battery.HighPerformance[1])
                                         })
                            model.append({
                                             "item_label": dsTr("Custom"),
                                             "item_value": 0,
                                             "item_tooltip": dsTr("Turn off the display") + ": " + formatTime(planInfo.Battery.Custom[0]) + "\n" +
                                             dsTr("Suspend") + ": " + formatTime(planInfo.Battery.Custom[1])
                                         })
                            return model
                        }

                        delegate: PropertyItem {
                            currentValue: dbus_power.batteryPlan
                            onSelectAction: {
                                dbus_power.batteryPlan = itemValue
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: dbus_power.batteryPlan == 0 ? battery_custom_column.height + arrow_rect_2.arrowHeight : 0
                visible: dbus_power.batteryPlan == 0 && dbus_power.batteryIsPresent
                color: dconstants.contentBgColor

                Behavior on height {
                    SmoothedAnimation { duration: 300 }
                }

                ArrowRect {
                    id: arrow_rect_2
                    fillStyle: dconstants.bgColor
                    stroke: false
                    radius: 0
                    lineWidth: 0
                    arrowPosition: 0.75

                    anchors.fill: parent
                }

                Column {
                    id: battery_custom_column
                    width: parent.width
                    anchors.top: parent.top
                    anchors.topMargin: arrow_rect_2.arrowHeight

                    TitleSeparator {
                        text: dsTr("Custom Options")
                    }
                    TitleSeparator {
                        width: parent.width
                        color: constants.contentBgColor
                        text: dsTr("Turn off the display")
                    }

                    Rectangle {
                        width: parent.width
                        height: 30
                        color: constants.contentBgColor
                        GridView{
                            id: battery_turn_off_monitor_view
                            anchors.fill: parent
                            anchors.leftMargin: 20
                            anchors.rightMargin: 20

                            cellWidth: width/7
                            cellHeight: 30

                            model: {
                                var model = listModelComponent.createObject(battery_turn_off_monitor_view, {})
                                for(var i=0; i<7; i++){
                                    model.append({
                                                     "item_label": indexToLabel(i),
                                                     "item_value": indexToTimeout(i),
                                                     "item_tooltip": ""
                                                 })
                                }
                                return model
                            }

                            delegate: PropertyItem {
                                currentValue: dbus_power.batteryIdleDelay
                                onSelectAction: {
                                    dbus_power.batteryPlan = 0
                                    dbus_power.batteryIdleDelay = itemValue
                                }
                            }
                        }
                    }
                    TitleSeparator {
                        width: parent.width
                        color: dconstants.contentBgColor
                        text: dsTr("Suspend")
                    }
                    Rectangle {
                        width: parent.width
                        height: 30
                        color: dconstants.contentBgColor
                        GridView{
                            id: battery_suspend_view
                            anchors.fill: parent
                            anchors.leftMargin: 20
                            anchors.rightMargin: 20


                            cellWidth: width/7
                            cellHeight: 30

                            model: {
                                var model = listModelComponent.createObject(battery_suspend_view, {})
                                for(var i=0; i<7; i++){
                                    model.append({
                                                     "item_label": indexToLabel(i),
                                                     "item_value": indexToTimeout(i),
                                                     "item_tooltip": ""
                                                 })
                                }
                                return model
                            }

                            delegate: PropertyItem {
                                currentValue: dbus_power.batterySuspendDelay
                                onSelectAction: {
                                    dbus_power.batteryPlan = 0
                                    dbus_power.batterySuspendDelay = itemValue
                                }
                            }
                        }
                    }

                }
            }
            DSeparatorHorizontal {}
            AppletSwitchArea {}
        }
    }
}
