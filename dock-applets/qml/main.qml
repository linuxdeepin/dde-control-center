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
import DBus.Com.Deepin.Dde.ControlCenter 1.0
import DBus.Com.Deepin.Daemon.DiskMount 1.0
import DBus.Com.Deepin.Daemon.Power 1.0
import DBus.Com.Deepin.Daemon.Network 1.0
import DBus.Com.Deepin.Daemon.Dock 1.0
import Helper 1.0

QtObject {
    id: root

    property var dconstants: DConstants {}
    property var dbusDockSetting: DockSetting {
        path: "/dde/dock/DockSetting"
        onDisplayModeChanged: {
            root.dockDisplayMode = arg0
        }
    }
    property int dockDisplayMode: dbusDockSetting.GetDisplayMode()

    property var appletList: new Array()

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

    property var lastStateInfos: mainObject.getAppletVisibleFromConfig()

    Component.onCompleted: {
        init_applet_list_model()
    }

    signal appletInfosChanged

    property var appletListModel: ListModel {}

    // start init dbus backend
    // ControlCenter
    property var dbusControlCenter: ControlCenter{}

    // DiskMount
    property var dbusDiskMount: DiskMount {}
    property var mountDiskList: {
        var diskList = dbusDiskMount.diskList
        var mounts = new Array()
        for(var i in dbusDiskMount.diskList){
            if(diskList[i][2]){
                mounts.push(diskList[i])
            }
        }
        return mounts
    }

    onMountDiskListChanged: {
        var add = mountDiskList.length > 0
        update_applet_list("disk_mount", add)
    }

    // Power
    property var dbusPower: Power{
        onBatteryIsPresentChanged:{
            update_applet_list("power", batteryIsPresent)
        }
    }

    // NetworkManager
    property var dbusNetwork: NetworkManager {
        onNeedSecrets:{
            print("NeedSectets Emit in dss Frame:", arg0, arg1, arg2)
            if(!dbusControlCenter.isNetworkCanShowPassword()){
                passwordWindow.accessPointObj = arg0
                passwordWindow.accessPointEncryption = arg1
                passwordWindow.accessPointName = arg2
                passwordWindow.showDialog()
            }
        }
    }
    property var nmDevices: unmarshalJSON(dbusNetwork.devices)
    property var wiredDevices: nmDevices["wired"] == undefined ? [] : nmDevices["wired"]
    property var wirelessDevices: nmDevices["wireless"] == undefined ? [] : nmDevices["wireless"]

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

    function getBool(s){
        if(typeof(s) == "undefined"){
            return false
        }
        else{
            return s
        }
    }

    property var passwordWindow: WifiPasswordWindow {}

    // finish init dbus backend

    property var dssLocale: DLocale{
        domain: "dde-control-center"
    }

    function dsTr(s){
        return dssLocale.dsTr(s)
    }

    property var externalObject: ExternalObject {}
    function getIconUrl(path){
        return externalObject.getIconUrl(path)
    }

    function getIndexFromArray(s, a){
        for(var i in a){
            if(a[i] == s){
                return i
            }
        }
        return -1
    }

    function getAppletIndex(applet_id){
        for(var i=0; i<appletListModel.count; i++){
            if(applet_id == appletListModel.get(i).applet_id){
                return i
            }
        }
        return -1
    }

    function init_applet_list_model(){
        appletListModel.append({
            "applet_id": "network"
        })

        appletListModel.append({
            "applet_id": "sound"
        })

        appletListModel.append({
            "applet_id": "date_time"
        })

        if (dbusPower.batteryIsPresent){
            appletListModel.append({
                "applet_id": "power"
            })
        }
    }

    function update_applet_list(name, add){
        var i = getAppletIndex(name)
        if(add){
            if (i == "-1"){
                appletListModel.append({
                    "applet_id": name
                })
            }
        }
        else{
            if (i != "-1"){
                appletListModel.remove(i, 1)
            }
        }
        print("==> [info] Status Changed:", name, add)
        root.appletInfosChanged()
    }

    function set_hide_applet(name){
        print("set_hide_applet: " + name)
        var i = getAppletIndex(name)
        if(i != "-1"){
            repeater.itemAt(i).hide()
            root.appletInfosChanged()
        }
        else{
            print("Unknown applet name: " + name)
        }
        mainObject.setAppletVisibleToConfig(get_applet_infos())
    }

    function set_show_applet(name){
        print("set_show_applet: " + name)
        var i = getAppletIndex(name)
        if(i != "-1"){
            repeater.itemAt(i).show()
            root.appletInfosChanged()
        }
        else{
            print("Unknown applet name: " + name)
        }
        mainObject.setAppletVisibleToConfig(get_applet_infos())
    }

    function toggle_applet(applet_id){
        for(var i=0; i<appletListModel.count; i++){
            var id = appletListModel.get(i).applet_id
            if(id == applet_id){
                repeater.itemAt(i).toggle()
                root.appletInfosChanged()
            }
        }
        mainObject.setAppletVisibleToConfig(get_applet_infos())
    }

    function get_applet_infos(){
        var applet_infos = new Array()
        for(var i=0; i<appletListModel.count; i++){
            var info = new Array()
            var id = appletListModel.get(i).applet_id
            if(id=="date_time" || id=="disk_mount"){
                continue
            }
            info.push(id)
            info.push(appletNames[id])
            info.push(repeater.itemAt(i).source != "")
            applet_infos.push(info)
        }
        return marshalJSON(applet_infos)
    }

    property var applets: Item {
        Repeater {
            id: repeater
            model: appletListModel
            delegate: AppletLoader {
            }
        }
    }

}
