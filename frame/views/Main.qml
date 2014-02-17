import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Locale 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0
import DBus.Com.Deepin.Daemon.InputDevices 1.0

Item {
    id: root
    width: 1
    height: 1

    property int panelWidth: 360
    property int trayWidth: 48

    property bool clickedToHide: true
    property var displayId: Display {}
    property var toolTip: ToolTip {}
    property var inputDevicesId: InputDevices {}

    property var inputDevices: {
        var devices = new Object()
        for(var i in inputDevicesId.devInfoList){
            var path = inputDevicesId.devInfoList[i][0]
            var type = inputDevicesId.devInfoList[i][1]
            if(typeof devices[type] == "undefined"){
                var paths = new Array()
                paths.push(path)
                devices[type] = paths
            }
            else{
                devices[type].push(path)
            }
        }
        return devices
    }

    QtObject {
        id: screenSize
        property int x: displayId.primaryRect[0]
        property int y: displayId.primaryRect[1]
        property int width: displayId.primaryRect[2]
        property int height: displayId.primaryRect[3]
    }

    DLocale {
        id: dsslocale
        domain: "deepin-system-settings"
        dirname: "../../locale"
    }

    function dsTr(s){
        return dsslocale.dsTr(s)
    }

    PanelWindow {
        id: rootWindow
    }

    TipWindow {
        id: trayIconTip
        x: screenSize.x + screenSize.width - rootWindow.displayWidth - width
    }

    Component.onCompleted: {
        rootWindow.show()
    }

    // debug mode
    function showModule(modulesId){
        rootWindow.showModule(modulesId)
    }
    // debug mode

    function displayTrayIcon(){
        rootWindow.showTrayOrPanel()
    }

    function outerAreaClicked(mousex, mousey){
        if (clickedToHide){
            if ((rootWindow.displayWidth == trayWidth) && (
                !in_visible_area(mousex, mousey))) {
                rootWindow.hideTrayIcon()
            }
            else if ((rootWindow.displayWidth == panelWidth) && (
                !in_visible_area(mousex, mousey))) {
                rootWindow.hideTrayIcon()
            }
        }
    }

    function in_visible_area(mousex, mousey){
        var width = rootWindow.displayWidth
        var height = rootWindow.height
        if ((mousex > screenSize.x + screenSize.width - width) && (
        mousex < screenSize.x + screenSize.width) && (
        mousey > screenSize.y + screenSize.height - height) && (
        mousey < screenSize.y + screenSize.height)){
            return true
        }
        else {
            return false
        }
    }

}
