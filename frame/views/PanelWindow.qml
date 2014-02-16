import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Widgets 1.0
import QtGraphicalEffects 1.0

Window {
    id: rootWindow
    color: "transparent"
    flags: Qt.Popup | Qt.WindowStaysOnTopHint

    x: screenSize.x + screenSize.width - width
    y: screenSize.y
    width: 0
    //width: panelWidth
    height: screenSize.height

    property int displayWidth: 0
    //property int displayWidth: panelWidth

    property var dconstants: DConstants {}
    property var listModelComponent: DListModelComponent {}
    property bool clickedToHide: true

    // debug mode
    function showModule(modulesId){
        clickedToHide = false
        if(modulesId != "all"){
            panelContent.moduleLoaderItem.iconId = modulesId
            panelContent.moduleBox.x = trayWidth
        }
        if(!showAll.running){
            showAll.start()
        }
    }
    // debug mode

    function showTrayOrPanel() {
        if(clickedToHide){
            if(panelContent.moduleLoaderItem.iconId != ''){
                panelContent.moduleBox.x = trayWidth
            }
            if(!showAll.running && rootWindow.width != panelWidth){
                showAll.start()
            }
        }
    }

    function hideTrayIcon() {
        if(clickedToHide){
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
    }

    Timer {
        id: resetTimer
        running: false
        interval: 1000 * 10
        repeat: false
        onTriggered: {
            print(">>>>> reset")
            panelContent.moduleLoaderItem.iconId = ''
            panelContent.initTrayIcon()
            panelContent.moduleBox.x = panelContent.width
            panelContent.inDssHome = true
        }
    }

    PropertyAnimation {
        id: "hideAll"
        duration: 400
        target: rootWindow
        properties: "displayWidth"
        to: 0

        onStopped: {
            rootWindow.width = 0
            rootWindow.hide()
            if(panelContent.moduleLoaderItem.iconId != ''){
                //resetTimer.restart()
            }
            else {
                panelContent.initTrayIcon()
            }
        }
    }

    PropertyAnimation {
        id: "showTray"
        duration: 200
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
            rootWindow.width = screenSize.width
            rootWindow.show()
        }
    }

    Rectangle {
        id: frame
        color: dconstants.bgColor

        width: panelWidth
        height: parent.height
        x: rootWindow.width - displayWidth

        PanelContent {
            id: panelContent
            width: parent.width
            height: parent.height
        }
    }

    Image {
        anchors.right: frame.left
        width: 16
        height: parent.height
        source: "images/shadow.png"
    }

    //Rectangle {
        //id: borderLine
        //width: 16
        //height: parent.height
        //anchors.right: frame.left
        //color: "transparent"

        //Rectangle {
            //id: canvas
            //width: 1
            //height: parent.height
            //anchors.right: parent.right
            //color: "#3c3c3c"
        //}

    //}

    //Glow {
        //anchors.fill: borderLine
        //radius: 8
        //samples: 8
        //color: "black"
        //source: borderLine
    //}

}
