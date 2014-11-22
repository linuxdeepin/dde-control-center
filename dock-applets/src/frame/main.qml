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
import Deepin.Widgets 1.0
import QtQuick.Window 2.1
import Deepin.Locale 1.0
import Deepin.DockApplet 1.0
import Deepin.AppletWidgets 1.0
import DBus.Com.Deepin.Dde.ControlCenter 1.0
import DBus.Com.Deepin.Daemon.Network 1.0
import DBus.Com.Deepin.Daemon.Dock 1.0

QtObject {
    id: root

    property var dconstants: DConstants {}
    property var appletListModel: ListModel {}
    property var dssLocale: DLocale{
        domain: "dde-control-center"
    }

    function dsTr(s){
        return dssLocale.dsTr(s)
    }

    function getIconUrl(path){
        return mainObject.getIconUrl(path)
    }

    function marshalJSON(value){
        var valueJSON = JSON.stringify(value);
        return valueJSON
    }

    function unmarshalJSON(valueJSON) {
        if (!valueJSON) {
            print("==> [ERROR] unmarshalJSON", valueJSON)
        }
        var value = JSON.parse(valueJSON)
        return value
    }

    function getInitAppletSate(appletId){
        return mainObject.getAppletState(dockDisplayMode, appletId)
    }

    property var appletInfos: ListModel {
        function update(applet_id, applet_name, applet_visible,applet_icon){
            print("==> [info] update applet info:", applet_id)
            for(var i=0;i<count;i++){
                var tmpInfo = get(i)
                if(tmpInfo.applet_id == applet_id){
                    get(i).applet_name = applet_name
                    get(i).applet_visible = applet_visible
                    get(i).applet_icon = applet_icon
                    mainObject.setAppletState(dockDisplayMode, applet_id, applet_visible)
                    return
                }
            }

            append({
                       "applet_id": applet_id,
                       "applet_name": applet_name,
                       "applet_visible": applet_visible,
                       "applet_icon":applet_icon,
                       "setting_enable":true    //default all show in setting panel
                   })

            mainObject.setAppletState(dockDisplayMode, applet_id, applet_visible)
            return
        }

        function rmItem(applet_id){
            print("==> [info] remove item from appletInfos:", applet_id)
            for(var i=0;i<count;i++){
                var tmpInfo = get(i)
                if(tmpInfo.applet_id == applet_id){
                    root.setAppletState(applet_id, false)
                    remove(i)
                    return
                }
            }
        }
    }

    function exportAppletStates() {
        var results = []
        for(var i=0;i<appletInfos.count;i++){
            var tmp = []
            tmp.push(appletInfos.get(i).applet_id)
            tmp.push(appletInfos.get(i).applet_name)
            tmp.push(appletInfos.get(i).applet_visible)
            results.push(tmp)
        }

        return marshalJSON(results)
    }

    function setAppletState(applet_id, newState){
        for(var i=0; i<repeater.count; i++){
            //try sub item
            repeater.itemAt(i).item.setSubAppletState(applet_id,newState)
            if(repeater.itemAt(i).appletId == applet_id){
                repeater.itemAt(i).setAppletState(newState)
                break
            }
        }
    }

    function toggleAppletState(applet_id){
        for(var i=0; i<repeater.count; i++){
            if(repeater.itemAt(i).appletId == applet_id){
                repeater.itemAt(i).toggleAppletState()
                break
            }
        }
    }

    function showSettingWindow(mouseX) {
        appletSettingWindow.item.mouseX = mouseX
        appletSettingWindow.item.showWindow()
    }

    // init dock setting
    property var dbusDockSetting: DockSetting {
        path: "/dde/dock/DockSetting"
        onDisplayModeChanged: {
            root.dockDisplayMode = arg0
        }
    }
    property int dockDisplayMode: dbusDockSetting.GetDisplayMode()
    // init dock setting

    // ControlCenter
    property var dbusControlCenter: ControlCenter{}

    property var dbusNetwork: NetworkManager{}
    property var nmDevices: JSON.parse(dbusNetwork.devices)
    property var wiredDevices: nmDevices["wired"] == undefined ? [] : nmDevices["wired"]
    property var wirelessDevices: nmDevices["wireless"] == undefined ? [] : nmDevices["wireless"]

    property var passwordWindow: Loader {
        sourceComponent: WifiPasswordWindow {}
        active: wirelessDevices.length > 0
    }

    property var appletSettingWindow: Loader {
        sourceComponent: AppletSettingWindow {
            switchList:appletInfos
            onItemClicked: {
                root.setAppletState(switchTitle,switchState)
            }
        }

        active: true
    }

    property var applets: Item {
        Repeater {
            id: repeater
            model: appletListModel
            delegate: AppletLoader {}
        }
    }

    function initAppletListModel(){
        var applet_infos = mainObject.scanPlugins()
        for(var i=0;i<applet_infos.length;i++){
            if(i % 2 == 0){
                appletListModel.append({
                                           "applet_id": applet_infos[i],
                                           "applet_path": applet_infos[i+1]
                                       })
            }
        }
    }

    Component.onCompleted: {
        initAppletListModel()
    }
}

/*****
QtObject {
    property var appletNames: {
        "network": dsTr("Network"),
        "sound": dsTr("Sound"),
        "power": dsTr("Power"),
        "disk_mount": dsTr("Disk Mount"),
        "date_time": dsTr("Date and Time")
    }

    property var undockableApplet: {
        var a = new Array()
        a.push("network")
        a.push("sound")
        a.push("power")
        return a
    }

    //property var lastStateInfos: mainObject.getAppletVisibleFromConfig()




    function getIndexFromArray(s, a){
        for(var i in a){
            if(a[i] == s){
                return i
            }
        }
        return -1
    }




    function update_applet_list(name, add){
//        var i = getAppletIndex(name)
//        if(add){
//            if (i == "-1"){
//                appletListModel.append({
//                    "applet_id": name
//                })
//            }
//        }
//        else{
//            if (i != "-1"){
//                appletListModel.remove(i, 1)
//            }
//        }
//        print("==> [info] Status Changed:", name, add)
//        root.appletInfosChanged()
    }

    function set_hide_applet(name){
        print("set_hide_applet: " + name)
        var i = getAppletIndex(name)
        if(i != "-1"){
            //repeater.itemAt(i).hide()
            //root.appletInfosChanged()
        }
        else{
            print("Unknown applet name: " + name)
        }
        //mainObject.setAppletVisibleToConfig(get_applet_infos())
    }

    function set_show_applet(name){
        print("set_show_applet: " + name)
        var i = getAppletIndex(name)
        if(i != "-1"){
            //repeater.itemAt(i).show()
            //root.appletInfosChanged()
        }
        else{
            print("Unknown applet name: " + name)
        }
        //mainObject.setAppletVisibleToConfig(get_applet_infos())
    }

    function toggle_applet(applet_id){
        for(var i=0; i<appletListModel.count; i++){
            var id = appletListModel.get(i).applet_id
            if(id == applet_id){
                var appletItem = repeater.itemAt(i)
                // appletItem.setAppletState(!appletItem.show)
                // root.appletInfosChanged()
            }
        }
        // mainObject.setAppletVisibleToConfig(get_applet_infos())
    }



}
****/
