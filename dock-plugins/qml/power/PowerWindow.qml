import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Power 1.0

DockQuickWindow {
    id: root
    title: "DSS"
    appid: "Applet100663308"
    icon: iconPath
    width: 320; height: 200
    color: Qt.rgba(0, 0, 0, 0.85)

    //Timer{
        //id: testIcon
        //running: true
        //interval: 1000
        //repeat: true
        //property int counter: 0
        //property var powerPictures: ["0", "20", "40", "60", "80", "100"]
        //onTriggered:{
            //root.iconPath = "images/power_%1.png".arg(powerPictures[counter])
            //if(counter == powerPictures.length - 1){
                //counter = 0
            //}
            //else{
                //counter += 1
            //}
            //print(root.iconPath)
        //}
    //}

    property url iconPath: getIconPath()
    property var dbusPower: Power{
        onBatteryPercentageChanged:{
            root.iconPath = getIconPath()
        }
    }

    function getIconPath(){
        var percentage = dbusPower.batteryPercentage
        if(percentage < 5){
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
        else if(percentage >= 95 && percentage < 100){
            return "images/power_100.png"
        }
        else if(percentage == 100){
            return "images/power_100_full.png"
        }
    }

    Component.onCompleted: root.show()
}
