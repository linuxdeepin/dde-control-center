import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.Locale 1.0
import DBus.Com.Deepin.Dde.ControlCenter 1.0
import DBus.Com.Deepin.Daemon.DiskMount 1.0
import DBus.Com.Deepin.Daemon.Power 1.0
import DBus.Com.Deepin.Daemon.Network 1.0

QtObject {
    id: root

    property var appletList: new Array()

    Component.onCompleted: {
        init_applet_list_model()
    }

    property var appletListModel: ListModel {}

    // start init dbus backend
    property var dbusControlCenter: ControlCenter{}

    property var dbusDiskMount: DiskMount {}
    property var mountDiskList: {
        var diskList = dbusDiskMount.diskList
        var mounts = new Array()
        for(var i in dbusDiskMount.diskList){
            if(diskList[i][3]){
                mounts.push(diskList[i])
            }
        }
        return mounts
    }

    onMountDiskListChanged: {
        var add = mountDiskList.length > 0
        update_applet_list("disk_mount", add)
    }

    property var dbusPower: Power{
        onBatteryIsPresentChanged:{
            update_applet_list("power", batteryIsPresent)
        }
    }
    property var dbusNetwork: NetworkManager{
        onWirelessDevicesChanged: {
            wirelessDeviceList = getWirelessDeviceList()
        }
    }
    function getWirelessDeviceList(){
        var r = new Array()
        var devices = dbusNetwork.wirelessDevices
        for(var i in devices){
            r.push(devices[i][0])
        }
        return r
    }

    property var wirelessDeviceList: getWirelessDeviceList()

    onWirelessDeviceListChanged: {
        var add = wirelessDeviceList.length > 0
        update_applet_list("network", add)
    }
    // finish init dbus backend

    property var dssLocale: DLocale{
        domain: "deepin-system-settings"
        dirname: "../locale"
    }

    function dsTr(s){
        return dssLocale.dsTr(s)
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
            "applet_id": "dss"
        })

        if (dbusPower.batteryIsPresent){
            appletListModel.append({
                "applet_id": "power"
            })
        }

        appletListModel.append({
            "applet_id": "sound"
        })

        //if (dbusNetwork.wirelessDevices){
            //appletListModel.append({
                //"applet_id": "network"
            //})
        //}

        //if (mountDiskList.length > 0) {
            //appletListModel.append({
                //"applet_id": "disk_mount"
            //})
        //}
    }

    function update_applet_list(name, add){
        print(">> Dock Applet Status Changed:", name, add)
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
    }

    function set_hide_applet(name){
        print("set_hide_applet: " + name)
        var i = getAppletIndex(name)
        if(i != "-1"){
            repeater.itemAt(i).hide()
        }
        else{
            print("Unknown applet name: " + name)
        }
    }

    function set_show_applet(name){
        print("set_show_applet: " + name)
        var i = getAppletIndex(name)
        if(i != "-1"){
            repeater.itemAt(i).show()
        }
        else{
            print("Unknown applet name: " + name)
        }
    }

    function get_applet_list(){
        var applet_list = new Array()
        for(var i=0; i<appletListModel.count; i++){
            applet_list.push(appletListModel.get(i).applet_id)
        }
        return applet_list
    }

    property var applets: Item {
        Repeater {
            id: repeater
            model: appletListModel
            delegate: AppletLoader {
                qmlPath: "%1/main.qml".arg(applet_id)
            }
        }
    }
}
