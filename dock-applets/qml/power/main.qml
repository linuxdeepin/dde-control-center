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

    function showPower(){
        dbusControlCenter.ShowModule("power")
    }

    function hidePower(){
        set_hide_applet("power")
    }

    onActivate: {
        showPower()
    }

    menu: Menu{
        Component.onCompleted: {
            addItem("_Run", showPower);
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
