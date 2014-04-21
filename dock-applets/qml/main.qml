import QtQuick 2.0
import QtQuick.Window 2.1

QtObject {
    id: root

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
        qmlPath: "dss/DssWindow.qml"
    }

    property var networkLoader: AppletLoader{
        qmlPath: "network/network.qml"
    }
}
