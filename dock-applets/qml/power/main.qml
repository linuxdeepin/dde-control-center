import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import "../widgets"

DockApplet {
    id: root
    title: {
        if (dbusPower.onBattery || dbusPower.batteryPercentage == 100){
            return parseInt(dbusPower.batteryPercentage) + "%"
        }
        else{
            return "%1% On Changing...".arg(parseInt(dbusPower.batteryPercentage))
        }
    }
    appid: "AppletPower"
    icon: iconPath
    width: 260; height: 30

    property url iconPath: getIconPath()

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

    function getIconPath(){
        var percentage = parseInt(dbusPower.batteryPercentage)
        if (!dbusPower.onBattery){
            return "images/power_100_full.png"
        }
        else if(percentage <= 10){
            return "images/power_0.png"
        }
        else if(percentage <= 30){
            return "images/power_20.png"
        }
        else if(percentage <= 50){
            return "images/power_40.png"
        }
        else if(percentage <= 70){
            return "images/power_60.png"
        }
        else if(percentage <= 90){
            return "images/power_80.png"
        }
        else if(percentage <= 100){
            return "images/power_100.png"
        }
    }
}
