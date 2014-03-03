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
    height: screenSize.height

    property int displayWidth: 0

    property var dconstants: DConstants {}
    property var listModelComponent: DListModelComponent {}
    property bool clickedToHide: true

    // debug mode
    function showModule(moduleId){
        //clickedToHide = false
        if(moduleId != "all"){
            panelContent.moduleIconList.iconClickAction(moduleId)
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
                if(showAll.running){
                    showAll.stop()
                }
                hideAll.start()
            }
        }
    }

    PropertyAnimation {
        id: hideAll
        duration: 150
        target: rootWindow
        properties: "displayWidth"
        to: 0

        onStopped: {
            rootWindow.width = 0
            rootWindow.hide()
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
