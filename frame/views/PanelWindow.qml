import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Widgets 1.0
import Deepin.Window 1.0

DOverrideWindow {
    id: rootWindow
    color: "transparent"
    flags: Qt.Popup

    x: screenSize.x + screenSize.width - width
    y: screenSize.y
    width: 0
    height: screenSize.height

    property int displayWidth: 0

    signal moduleStartChange(string nextModuleName)
    signal panelHided

    property var dconstants: DConstants {}
    property var listModelComponent: DListModelComponent {}
    property bool clickedToHide: true

    function showModule(moduleId){
        panelContent.moduleIconList.iconClickAction(moduleId)
    }

    function showPanel() {
        if(clickedToHide){
            if(!showAll.running && rootWindow.width != panelWidth){
                showAll.start()
            }
        }
    }

    function hidePanel(quit) {
        if(clickedToHide){
            if(!hideAll.running){
                if(showAll.running){
                    showAll.stop()
                }
                hideAll.quit = quit
                hideAll.start()
            }
        }
    }

    function showAllImmediately(){
        rootWindow.width = panelWidth + 16
        rootWindow.show()
        rootWindow.raise()
        rootWindow.displayWidth = panelWidth
    }

    function hideAllImmediately(){
        rootWindow.width = 0
        rootWindow.hide()
        rootWindow.displayWidth = 0
    }

    function togglePanel(){
        if(rootWindow.displayWidth == 0){
            rootWindow.showPanel()
        }
        else{
            rootWindow.hidePanel(false)
        }
    }

    Timer {
        id: timeoutQuit
        running: false
        repeat: false
        interval: 1000 * 60 * 30
        onTriggered: {
            Qt.quit()
        }
    }

    PropertyAnimation {
        id: hideAll
        duration: 150
        target: rootWindow
        properties: "displayWidth"
        to: 0

        property bool quit: false

        onStopped: {
            rootWindow.width = 0
            rootWindow.hide()
            timeoutQuit.restart()
            if(quit){
                Qt.quit()
            }
            rootWindow.panelHided()
        }
    }

    SmoothedAnimation {
        id: showAll
        duration: 150
        target: rootWindow
        properties: "displayWidth"
        to: panelWidth

        onStarted: {
            rootWindow.width = panelWidth + 16
            rootWindow.show()
            rootWindow.raise()
            if(timeoutQuit.running){
                timeoutQuit.stop()
            }
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
}
