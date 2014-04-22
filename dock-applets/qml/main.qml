import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.Locale 1.0
import DBus.Com.Deepin.Dde.ControlCenter 1.0

QtObject {
    id: root

    property var dbusControlCenter: ControlCenter{}
    property var appletList: mainObject.get_applet_list()

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
