import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Locale 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0
import DBus.Com.Deepin.Api.XMouseArea 1.0

QtObject {
    id: root

    Component.onCompleted: {
        dbusXMouseArea.RegisterFullScreen()
    }

    property int panelWidth: 360
    property int trayWidth: 48

    property bool clickedToHide: true
    property var displayId: Display {}
    property var dbusXMouseArea: XMouseArea {
        onButtonRelease: {
            if(arg3 != -1){
                outerAreaClicked(arg1, arg2)
            }
        }
        onMotionMove: {
            cursorPosition.x = arg0
            cursorPosition.y = arg1
        }
    }
    property var toolTip: ToolTip {}

    property var screenSize: QtObject {
        property int x: displayId.primaryRect[0]
        property int y: displayId.primaryRect[1]
        property int width: displayId.primaryRect[2]
        property int height: displayId.primaryRect[3]
    }
    property var cursorPosition: QtObject {
        property int x: 0
        property int y: 0
    }

    property var dsslocale: DLocale {
        domain: "deepin-system-settings"
        dirname: "../../locale"
    }

    property var rootWindow: PanelWindow {}

    property var trayIconTip: IconTip{
        x: screenSize.x + screenSize.width - rootWindow.displayWidth - width
    }

    property var timeoutHideDss: Timer{
        interval: 2000
        onTriggered: {
            rootWindow.hidePanel(true)
        }
    }

    function dsTr(s){
        return dsslocale.dsTr(s)
    }

    function getIndexFromArray(s, a){
        for(var i in a){
            if(a[i] == s){
                return i
            }
        }
        return -1
    }

    function showModule(modulesId){
        rootWindow.showModule(modulesId)
    }

    function showDss(seconds){
        rootWindow.showPanel()
        if(seconds > 0){
            timeoutHideDss.interval = 1000 * seconds
            timeoutHideDss.restart()
        }
    }

    function showDssImmediately(){
        rootWindow.showAllImmediately()
    }

    function hideDss(){
        rootWindow.hidePanel(false)
    }

    function hideDssImmediately(){
        rootWindow.hideAllImmediately()
    }

    function togglePanel(){
        rootWindow.togglePanel()
    }

    function outerAreaClicked(mousex, mousey){
        if (clickedToHide){
            if ((rootWindow.displayWidth == trayWidth) && (
                !in_visible_area(mousex, mousey))) {
                hideDss()
            }
            else if ((rootWindow.displayWidth == panelWidth) && (
                !in_visible_area(mousex, mousey))) {
                hideDss()
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
