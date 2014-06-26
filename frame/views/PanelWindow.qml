/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Kaisheng Ye
**
**  Author:     Kaisheng Ye <kaisheng.ye@gmail.com>
**  Maintainer: Kaisheng Ye <kaisheng.ye@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Widgets 1.0

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
    signal moduleIconClicked(string iconId)

    property var dconstants: DConstants {}
    property var listModelComponent: DListModelComponent {}
    property bool clickedToHide: true
    property alias panelContent: panelContent

    function showModule(moduleId){
        panelContent.moduleIconList.iconClickAction(moduleId)
    }

    function showPanel() {
        if(clickedToHide){
            if(!showAll.running && rootWindow.width != panelWidth){
                if(hideAll.running){
                    hideAll.stop()
                }
                showAll.start()
            }
        }
    }

    function hidePanel(quit) {
        if(clickedToHide){
            if(!hideAll.running && !showAll.running){
                hideAll.quit = quit
                hideAll.start()
            }
        }
    }

    function showAllImmediately(){
        rootWindow.width = panelWidth + 16
        rootWindow.show()
        rootWindow.raise()
        rootWindow.requestActivate()
        rootWindow.displayWidth = panelWidth
    }

    function hideAllImmediately(){
        rootWindow.width = 0
        rootWindow.hideWindow()
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

    function hideWindow(){
        rootWindow.hide()
        timeoutQuit.restart()
    }

    Timer {
        id: timeoutQuit
        running: false
        repeat: false
        interval: 1000 * 60 * 5 // 10 minutes to quit
        onTriggered: {
            Qt.quit()
        }
    }

    PropertyAnimation {
        id: hideAll
        duration: 300
        target: rootWindow
        properties: "displayWidth"
        to: 0
        easing.type: Easing.OutQuart

        property bool quit: false

        onStopped: {
            rootWindow.width = 0
            rootWindow.hideWindow()
            timeoutQuit.restart()
            if(quit){
                Qt.quit()
            }
            rootWindow.panelHided()
        }
    }

    SmoothedAnimation {
        id: showAll
        duration: 300
        target: rootWindow
        properties: "displayWidth"
        to: panelWidth
        easing.type: Easing.OutQuart

        onStarted: {
            rootWindow.width = panelWidth + 16
            rootWindow.show()
            rootWindow.raise()
            rootWindow.requestActivate()
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
