import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Power 1.0

DockApplet {
    id: root
    title: dbusPower.batteryPercentage + "%"
    appid: "AppletPower"
    icon: iconPath
    width: 260; height: 30

    property url iconPath: getIconPath()
    property var dbusPower: Power{
        onBatteryPercentageChanged: {
            root.iconPath = getIconPath()
        }
    }

    function getIconPath(){
        var percentage = parseInt(dbusPower.batteryPercentage)
        print("Debug:", percentage)
        if(percentage <= 5){
            return "images/power_0.png"
        }
        else if(percentage <= 20){
            return "images/power_20.png"
        }
        else if(percentage <= 40){
            return "images/power_40.png"
        }
        else if(percentage <= 60){
            return "images/power_60.png"
        }
        else if(percentage <= 80){
            return "images/power_80.png"
        }
        else if(percentage <= 95){
            return "images/power_100.png"
        }
        else if(percentage <= 100){
            return "images/power_100_full.png"
        }
    }
}
