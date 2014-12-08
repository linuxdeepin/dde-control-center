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
import Deepin.Locale 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0
import DBus.Com.Deepin.Api.XMouseArea 1.0
import DBus.Com.Deepin.Daemon.Network 1.0

QtObject {
    id: root

    Component.onCompleted: {
        dbusXMouseArea.RegisterFullScreen()
    }

    signal windowXChanged(int windowX)

    property int panelWidth: 360
    property int trayWidth: 48

    property bool clickedToHide: true
    property bool gotShowModuleFlag: false
    property var displayId: Display {}
    property var dbusXMouseArea: XMouseArea {
        onButtonRelease: {
            if(arg3 != -1){
                gotShowModuleFlag = false

                if (!in_visible_area(arg1,arg2) && clickedToHide)
                    clickedToHide = true
                else
                    clickedToHide = false

                delayHideTimer.mousex = arg1
                delayHideTimer.mousey = arg2
                delayHideTimer.start()
            }
        }
        onButtonPress: {
            //remeber press position,user maybe drag outside the control-center
            clickedToHide = !in_visible_area(arg1,arg2)
        }

        onCursorMove: {
            cursorPosition.x = arg0
            cursorPosition.y = arg1
        }
        onCancelAllArea: {
            RegisterFullScreen()
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
        domain: "dde-control-center"

        Component.onCompleted: print("==> [info] Language:", dsslocale.lang)
    }

    property var dbusNetwork: NetworkManager {
        onNeedSecrets:{
            print("NeedSectets Emit in dss Frame:", arg0, arg1, arg2,arg3)
            if(getNetworkCanShowPassword()){
                rootWindow.panelContent.rightBoxLoader.item.needSecretsEmit(arg0, arg1, arg2,arg3)
            }
        }
    }

    function getNetworkCanShowPassword() {
        return (rootWindow.displayWidth !=0 && rootWindow.panelContent.currentContentId == "network" && rootWindow.panelContent.rightBoxLoader.item.inAllConnectionPage)
    }

    function getWindowX() {
        return rootWindow.getRealWindowX()
    }

    function getShowInRightFlag(){
        return rootWindow.shouldShowInRight
    }

    property var rootWindow: PanelWindow {
        onWindowXChanged: {
            root.windowXChanged(windowX)
        }
    }

    property var timeoutHideDss: Timer{
        interval: 2000
        onTriggered: {
            rootWindow.hidePanel(true)
        }
    }

    property var delayHideTimer: Timer {
        property int mousex: 0
        property int mousey: 0

        interval: 200
        repeat: false
        onTriggered: {
            if (!gotShowModuleFlag){
                outerAreaClicked(mousex, mousey)
            }

        }
    }


    function getStringPixelSize(preText,fontPixelSize){
        var tmpText = calculateTextComponent.createObject(null, { "text": preText , "font.pixelSize": fontPixelSize})
        var width = tmpText.width
        return width
    }

    property var calculateTextComponent: Component {
        Text{visible: false}
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
        gotShowModuleFlag = true
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

    function togglePanelInLeft(){
        rootWindow.togglePanelInLeft()
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
        if (rootWindow.shouldShowInRight){
            if ((mousex >= screenSize.x + screenSize.width - width) && (
            mousex <= screenSize.x + screenSize.width) &&
            mousey >= screenSize.y && (
            mousey <= screenSize.y + screenSize.height)){
                return true
            }
            else {
                return false
            }
        }
        else{
            if ((mousex <= width + screenSize.x) &&
                 mousex >= screenSize.x &&
                 mousey >= screenSize.y && (
                 mousey <= screenSize.y + screenSize.height)){
                return true
            }
            else {
                return false
            }
        }
    }
}
