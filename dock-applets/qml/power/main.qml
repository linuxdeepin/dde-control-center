import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import "../widgets"

DockApplet {
    id: root
    title: {
        if (!dbusPower.onBattery && dbusPower.batteryState == 1){
            return dsTr("On Charging") + " %1%".arg(dbusPower.batteryPercentage)
        }
        else{
            return "%1%".arg(dbusPower.batteryPercentage)
        }
    }
    appid: "AppletPower"
    icon: getIcon()
    width: 260;
    height: 30

    property var planNames: {
        0: dsTr("Custom"),
        1: dsTr("Power saver"),
        2: dsTr("Balanced"),
        3: dsTr("High performance")
    }
    property int currentPlan: dbusPower.onBattery ? dbusPower.batteryPlan : dbusPower.linePowerPlan

    onCurrentPlanChanged: {
        menu.updateMenu()
    }

    function showPower(id){
        dbusControlCenter.ShowModule("power")
    }

    function hidePower(id){
        set_hide_applet("power")
    }

    onActivate: {
        showPower(0)
    }

    function do_nothing(id){
    }

    function changePowerPlan(id){
        var id_index = id.split(":")[2]
        var planIndex = menu.menuIds.indexOf(id_index)
        if(dbusPower.onBattery){
            dbusPower.batteryPlan = planIndex
        }
        else{
            dbusPower.linePowerPlan = planIndex
        }
    }

    menu: Menu {
        property var menuIds: new Array()

        function updateMenu(){
            for(var i in menuIds){
                var checked = false
                if(currentPlan == i){
                    checked = true
                }
                var content_obj = unmarshalJSON(content)
                content_obj.items[menuIds[i]].checked = checked
                content = marshalJSON(content_obj)
            }
        }

        Component.onCompleted: {
            addItem(dsTr("_Run"), showPower);
            addItem("", do_nothing);
            for(var i in planNames){
                var menuId = addCheckboxItem("power_plan", planNames[i], changePowerPlan);
                menuIds.push(menuId);
            }
            addItem("", do_nothing);
            addItem(dsTr("_Undock"), hidePower);
            updateMenu()
        }
    }

    function getIcon(){
        if (dbusPower.onBattery){
            var path = "battery-%1"
        }
        else{
            var path = "battery-charge-%1"
        }

        var percentage = parseInt(dbusPower.batteryPercentage)
        if(percentage <= 5){
            return path.arg(0)
        }
        else if(percentage <= 18){
            return path.arg(10)
        }
        else if(percentage <= 38){
            return path.arg(25)
        }
        else if(percentage <= 62){
            return path.arg(50)
        }
        else if(percentage <= 88){
            return path.arg(75)
        }
        else if(percentage <= 100){
            return path.arg(100)
        }
    }
}
