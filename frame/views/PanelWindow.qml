import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Locale 1.0
import Deepin.Widgets 1.0

Window {
    id: rootWindow
    color: "transparent"
    flags: Qt.Popup

    x: screenSize.x + screenSize.width - width
    y: screenSize.y
    width: 0
    height: screenSize.height

    property int displayWidth: 0

    property var dconstants: DConstants {}
    property var listModelComponent: DListModelComponent {}

    DLocale {
        id: dsslocale
        domain: "deepin-system-settings"
        dirname: "../../locale"
    }

    function dsTr(s){
        return dsslocale.dsTr(s)
    }

    function showTrayOrPanel() {
        if(panelContent.moduleLoaderItem.iconId == ''){
            if(!showTray.running){
                showTray.start()
            }
        }
        else if(panelContent.moduleLoaderItem.iconId != ''){
            if(!showAll.running){
                showAll.start()
            }
        }
    }

    function hideTrayIcon() {
        if(!hideAll.running){
            if(showTray.running){
                showTray.stop()
            }
            else if(showAll.running){
                showAll.stop()
            }
            hideAll.start()
        }
    }

    Timer {
        id: resetTimer
        running: false
        interval: 1000 * 10
        repeat: false
        onTriggered: {
            panelContent.moduleLoaderItem.iconId = ''
            panelContent.initTrayIcon()
        }
    }

    PropertyAnimation {
        id: "hideAll"
        duration: 300
        target: rootWindow
        properties: "displayWidth"
        to: 0

        onStopped: {
            rootWindow.width = 0
            rootWindow.hide()
            if(panelContent.moduleLoaderItem.iconId != ''){
                resetTimer.restart()
            }
            else {
                panelContent.initTrayIcon()
            }
        }
    }

    PropertyAnimation {
        id: "showTray"
        duration: 300
        target: rootWindow
        properties: "displayWidth"
        to: trayWidth

        onStarted: {
            if(resetTimer.running){
                resetTimer.stop()
            }
            rootWindow.width = panelWidth
            rootWindow.show()
        }
    }

    SmoothedAnimation {
        id: "showAll"
        duration: 300
        target: rootWindow
        properties: "displayWidth"
        to: panelWidth

        onStarted: {
            if(resetTimer.running){
                resetTimer.stop()
            }
            rootWindow.width = panelWidth
            rootWindow.show()
        }
    }

    Timer{
        //running: true
        interval: 2000
        repeat: true
        onTriggered: {
            if(frame.x == (screenSize.width - trayWidth)){
                frame.x = screenSize.width - panelWidth
            }
            else{
                frame.x = screenSize.width - trayWidth
            }
        }
    }

    Rectangle{
        id: frame
        color: dconstants.bgColor

        width: panelWidth
        height: parent.height
        x: rootWindow.width - displayWidth

        PanelContent {
            id: panelContent
        }
    }
}
