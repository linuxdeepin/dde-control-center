import QtQuick 2.0
import QtQuick.Window 2.1
import DBus.Com.Deepin.Dde.ControlCenter 1.0

QtObject {
    id: root

    property var dbusControlCenter: ControlCenter{}

    function set_hide_applet(name){
        print("set_hide_applet: " + name)
        switch(name)
        {
        case "dss":
            dssLoader.hide();
            break;
        case "power":
            powerLoader.hide();
            break;
        default:
            print("Unknown applet name: " + name)
        }
    }

    function set_show_applet(name){
        print("set_show_applet: " + name)
        switch(name)
        {
        case "dss":
            dssLoader.show();
            break;
        case "power":
            powerLoader.show();
            break;
        default:
            print("Unknown applet name: " + name)
        }
    }

    property var dssLoader: AppletLoader{
        qmlPath: "dss/main.qml"
    }

    property var networkLoader: AppletLoader{
        qmlPath: "network/main.qml"
    }

    property var soundLoader: AppletLoader{
        qmlPath: "sound/main.qml"
    }

    property var powerLoader: AppletLoader{
        qmlPath: "power/main.qml"
    }
}
