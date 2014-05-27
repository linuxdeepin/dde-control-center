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
        print(id)
    }

    menu: Menu{
        Component.onCompleted: {
            addItem("_Run", showPower);
            addItem("", do_nothing);
            addCheckboxItem("power_plan", dsTr("Balanced"), changePowerPlan);
            addCheckboxItem("power_plan", dsTr("Power saver"), changePowerPlan);
            addCheckboxItem("power_plan", dsTr("High performance"), changePowerPlan);
            addItem("", do_nothing);
            addItem("_Undock", hidePower);
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
