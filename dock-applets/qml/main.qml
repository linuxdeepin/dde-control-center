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
        appletList = get_applet_list()
    }

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
    property var dbusNetwork: NetworkManager{}
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

    function get_applet_list(){
        var applet_list = new Array()
        applet_list.push("dss")

        if (dbusPower.batteryIsPresent){
            applet_list.push("power")
        }

        applet_list.push("sound")

        if (dbusNetwork.wirelessDevices){
            applet_list.push("network")
        }

        if (mountDiskList.length > 0) {
            applet_list.push("disk_mount")
        }
        return applet_list
    }

    function update_applet_list(name, add){
        var i = getIndexFromArray(name, appletList)
        if(add){
            if (i == "-1"){
                appletList.push(name)
            }
        }
        else{
            if (i != "-1"){
                appletList.splice(i, 1)
            }
        }
    }

    function set_hide_applet(name){
        print("set_hide_applet: " + name)
        var i = getIndexFromArray(name, appletList)
        if(i != "-1"){
            repeater.itemAt(i).hide()
        }
        else{
            print("Unknown applet name: " + name)
        }
    }

    function set_show_applet(name){
        print("set_show_applet: " + name)
        var i = getIndexFromArray(name, appletList)
        if(i != "-1"){
            repeater.itemAt(i).show()
        }
        else{
            print("Unknown applet name: " + name)
        }
    }

    property var applets: Item {
        Repeater {
            id: repeater
            model: appletList
            delegate: AppletLoader {
                qmlPath: "%1/main.qml".arg(modelData)
            }
        }
    }
}
