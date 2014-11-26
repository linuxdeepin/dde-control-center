/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Kaisheng Ye
**
**  Author:     Kaisheng Ye <kaisheng.ye@gmail.com>
**  Maintainer: Kaisheng Ye <kaisheng.ye@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import Deepin.AppletWidgets 1.0

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
        setAppletState(false)
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

    menu: AppletMenu {
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
        var percentage = parseInt(dbusPower.batteryPercentage)
        if(dockDisplayMode == 0){
            if (dbusPower.onBattery){
                var path = "dock-battery-%1"
            }
            else{
                var path = "dock-battery-charge-%1"
            }

            if(percentage <= 5){
                return path.arg(0)
            }
            else if(percentage <= 18){
                return path.arg(1)
            }
            else if(percentage <= 38){
                return path.arg(2)
            }
            else if(percentage <= 62){
                return path.arg(3)
            }
            else if(percentage <= 88){
                return path.arg(4)
            }
            else{
                return path.arg(5)
            }
        }
        else{
            if (!dbusPower.onBattery){
                return "dock-battery-charge"
            }
            else {
                var winPath = "dock-battery-%1"
                if(percentage <= 5){
                    return winPath.arg(0)
                }
                else if(percentage <= 20){
                    return winPath.arg(1)
                }
                else if(percentage <= 40){
                    return winPath.arg(2)
                }
                else if(percentage <= 60){
                    return winPath.arg(3)
                }
                else if(percentage <= 80){
                    return winPath.arg(4)
                }
                else{
                    return winPath.arg(5)
                }
            }
        }
    }
}
