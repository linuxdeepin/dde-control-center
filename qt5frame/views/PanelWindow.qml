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
    color: Qt.rgba(0, 0, 0, 0)
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.X11BypassWindowManagerHint

    x: screenSize.x + screenSize.width - width
    y: screenSize.y
    width: validWidth ? panelWidth + moduleNameMaxWidth : 0
    height: screenSize.height

    property bool validWidth: false
    property int moduleNameMaxWidth: modulesId.getMaxModuleNameWidth()

    property int displayWidth: 0

    signal moduleStartChange(string nextModuleName)
    signal panelHided
    signal moduleIconClicked(string iconId)

    property var dconstants: DConstants {}
    property var listModelComponent: DListModelComponent {}
    property bool clickedToHide: true
    property alias panelContent: panelContent
    property var modulesId: ModulesData {}

    function showModule(moduleId){
        panelContent.moduleIconList.iconClickAction(moduleId)
    }

    function showPanel() {
        if(clickedToHide){
            if(!showAll.running && rootWindow.width != panelWidth && !hideAll.running){
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
        rootWindow.validWidth = true
        rootWindow.show()
        rootWindow.raise()
        rootWindow.requestActivate()
        rootWindow.displayWidth = panelWidth
    }

    function hideAllImmediately(){
        rootWindow.validWidth = false
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
        trayIconTip.visible = false
        timeoutQuit.restart()
    }

    function setPanelHidable(canHide){
        if(canHide){
            timerPanelCanHide.restart()
        }
        else{
            clickedToHide = false
        }
    }

    Timer {
        id: timerPanelCanHide
        running: false
        repeat: false
        interval: 800
        onTriggered: {
            clickedToHide = true
        }
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
            validWidth = false
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
            validWidth = true
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

    InnerIconTip {
        id: trayIconTip
        x: rootWindow.width - rootWindow.displayWidth - width - 6
    }

    Menu {
        id: rootMenu
    }

    function isInRect(pos, rect){
        if(pos.x > rect.x && pos.x < rect.x + rect.width && pos.y > rect.y && pos.y < rect.y + rect.height){
            return true
        }
        else{
            return false
        }
    }

    onWindowFocusChanged: {
        if(!window && rootMenu.visible){
            rootMenu.visible = false
        }
    }

    onMousePressed: {
        var pos = rootWindow.getCursorPos()
        pos.x = pos.x - rootWindow.x
        pos.y = pos.y - rootWindow.y
        if(!isInRect(pos, rootMenu)){
            rootMenu.visible = false
        }
    }
}
