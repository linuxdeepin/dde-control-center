import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Locale 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0

Item {
    id: root
    width: 1
    height: 1

    property int panelWidth: 360
    property int trayWidth: 48

    property bool clickedToHide: true
    property var displayId: Display {}
    property var toolTip: ToolTip {}

    //property var xmouseAreaId: XMouseArea {
        //onMotionCoordinate: {
            ////print(arg0, arg1, arg2, arg3)
        //}
    //}


    QtObject {
        id: screenSize
        property int x: displayId.primaryRect[0]
        property int y: displayId.primaryRect[1]
        property int width: displayId.primaryRect[2]
        property int height: displayId.primaryRect[3]
    }

    //property int showAreaId: {
        //var x1 = screenSize.x + screenSize.width - 30
        //var x2 = screenSize.x + screenSize.width
        //var y1 = screenSize.y + screenSize.height - 30
        //var y2 = screenSize.y + screenSize.height
        //print("RegisterArea:", x1, x2, y1, y2)
        //var areaId = xmouseAreaId.RegisterArea([(x1, x2, y1, y2)])
        //print("AreaId:", areaId)
        //return areaId
    //}

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

    Timer{
        id: timeoutHideDss
        repeat: false
        running: false
        interval: 2000
        onTriggered: {
            rootWindow.hideTrayIcon()
        }
    }

    function showModule(modulesId){
        rootWindow.showModule(modulesId)
    }

    function showDss(seconds){
        rootWindow.showTrayOrPanel()
        if(seconds > 0){
            timeoutHideDss.interval = 1000 * seconds
            timeoutHideDss.restart()
        }
    }

    function showDssImmediately(){
        rootWindow.showAllImmediately()
    }

    function hideDss(){
        rootWindow.hideTrayIcon()
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
        if ((mousex >= screenSize.x + screenSize.width - width) && (
        mousex <= screenSize.x + screenSize.width) && (
        mousey >= screenSize.y + screenSize.height - height) && (
        mousey <= screenSize.y + screenSize.height)){
            return true
        }
        else {
            return false
        }
    }

}
