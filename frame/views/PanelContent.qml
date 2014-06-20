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
import DBus.Com.Deepin.SessionManager 1.0
import DBus.Com.Deepin.Daemon.Accounts 1.0
import DBus.Com.Deepin.Daemon.InputDevices 1.0
import DBus.Com.Deepin.Daemon.Bluetooth 1.0

Rectangle {
    id: panelContent
    color: dconstants.bgColor

    property alias moduleLoaderItem: rightBoxLoaderItem
    property alias moduleBox: rightBox
    property alias moduleIconList: moduleIconList
    property alias rightBoxLoader: rightBoxLoader

    property var inputDevicesId: InputDevices {}
    property bool isTouchpadExist: {
        var devInfoList = inputDevicesId.infos
        for(var i in devInfoList){
            if(devInfoList[i][1] == 'touchpad'){
                return true
            }
        }
        return false
    }

    // bluetooth
    property var dbusBluetooth: Bluetooth {}
    property var isBluetoothExist: dbusBluetooth.adapters ? JSON.parse(dbusBluetooth.adapters).length > 0 : false

    onIsBluetoothExistChanged: {
        var index = navigateIconModel.getIndex("moduleId", "bluetooth")
        if(isBluetoothExist){
            if(index == -1){
                var i = navigateIconModel.getIndex("moduleId", "sound")
                if(i != -1){
                    var localeName = modulesId.moduleLocaleNames["bluetooth"]
                    navigateIconModel.insert(i, {
                        "moduleId": "bluetooth",
                        "moduleLocaleName": localeName
                    })
                }
            }
        }
        else{
            if(index != -1){
                navigateIconModel.remove(index)
            }
        }
    }

    property bool isSiderNavigate: false
    property string currentContentId: ""

    property var iconIdToIndex
    property color tuhaoColor: "#faca57"

    property var sessionManager: SessionManager {}
    property var modulesId: ModulesData {}

    property bool inDssHome: true
    property var navigateIconModel: ListModel {
        function getIndex(key, value){
            for(var i=0; i<count; i++){
                var obj = get(i)
                if(obj[key] == value){
                    return i
                }
            }
            return -1

        }
    }
    property real trayIconHeight: {
        if(navigateIconModel.count==0){
            return trayWidth
        }
        else{
            var new_tray_height = iconsArea.height/(navigateIconModel.count)
            return new_tray_height < trayWidth ? new_tray_height : trayWidth
        }
    }

    function initTrayIcon() {
        print("==> [info] initTrayIcon emit")
        var modules_id_array = modulesId.allIds
        moduleIconList.currentIndex = -1
        navigateIconModel.clear()

        for(var i in modules_id_array){
            var module_id = modules_id_array[i]
            if(module_id == "mouse_touchpad" && !isTouchpadExist){
                var localeName = modulesId.moduleLocaleNames["mouse"]
            }
            else if(module_id == "bluetooth" && !isBluetoothExist){
                continue
            }
            else{
                var localeName = modulesId.moduleLocaleNames[module_id]
            }

            navigateIconModel.append({
                "moduleId": module_id,
                "moduleLocaleName": localeName
            })
        }

        iconIdToIndex = new Object()
        for(var i=0; i<navigateIconModel.count; i++){
            iconIdToIndex[navigateIconModel.get(i).moduleId] = i
        }
    }

    function addHomeShutdownButton() {
        panelContent.isSiderNavigate = true

        var item = navigateIconModel.get(0)
        if(item.moduleId != "home"){
            navigateIconModel.insert(0, {
                "moduleId": "home",
                "moduleLocaleName": modulesId.moduleLocaleNames["home"]
            })
        }

        var item = navigateIconModel.get(navigateIconModel.count-1)
        if(item.moduleId != "shutdown"){
            navigateIconModel.append({
                "moduleId": "shutdown",
                "moduleLocaleName": modulesId.moduleLocaleNames["shutdown"]
            })
        }
    }

    function removeHomeShutdownButton() {
        panelContent.isSiderNavigate = false

        var item = navigateIconModel.get(0)
        if(item.moduleId == "home"){
            navigateIconModel.remove(0)
        }
        var item = navigateIconModel.get(navigateIconModel.count-1)
        if(item.moduleId == "shutdown"){
            navigateIconModel.remove(navigateIconModel.count - 1)
        }
    }

    function shutdownButtonClicked(){
        rootWindow.width = 0
        rootWindow.displayWidth = 0
        rootWindow.hide()
        sessionManager.PowerOffChoose()
    }

    function trayIconHoverHandler(module_id, index) {
        var tipDisplayHeight
        tipDisplayHeight = Math.abs(trayIconHeight - trayIconTip.height)/2 
            + trayIconHeight * index
        if (trayIconHeight == trayWidth) {
        }
        trayIconTip.y = tipDisplayHeight
        if(module_id == "mouse_touchpad" && !isTouchpadExist){
            var localeName = modulesId.moduleLocaleNames["mouse"]
        }
        else{
            var localeName = modulesId.moduleLocaleNames[module_id]
        }
        trayIconTip.text = localeName
        if(isSiderNavigate){
            trayIconTip.visible = true
        }
        else{
            trayIconTip.visible = false
        }
    }

    Timer {
        running: true
        repeat: false
        interval: 100
        onTriggered: {
            initTrayIcon()
        }
    }

    ParallelAnimation {
        id: toSiderNavigateAnimation
        PropertyAnimation {
            target: headerArea
            property: "y"
            to: 0 - headerArea.height
            duration: 200
        }

        PropertyAnimation {
            target: footerArea
            property: "anchors.bottomMargin"
            to: -footerArea.height
            duration: 200
        }
        PropertyAnimation {
            target: iconsArea
            property: "opacity"
            to: 0
            duration: 200
        }

        onStopped: {
            showContentPanelTimer.start()
        }
    }

    Timer {
        id: showContentPanelTimer
        interval: 100
        onTriggered: {
            iconsArea.width = 48
            iconsArea.anchors.topMargin = 0
            //listBox.width = 48
            moduleIconList.itemSize = 48
            addHomeShutdownButton()
            rightBoxLoaderItem.iconId = currentContentId
            moduleIconList.iconLabelOpacity = 0
            rightBox.x = 48
            showContentPanel.start()
        }
    }

    ParallelAnimation {
        id: showContentPanel
        PropertyAnimation {
            target: iconsArea
            property: "opacity"
            to: 1
            duration: 200
        }
    }

    ParallelAnimation {
        id: toGridNavigateAnimation
        PropertyAnimation {
            target: iconsArea
            property: "opacity"
            to: 0
            duration: 200
        }

        onStopped: {
            iconsArea.width = panelContent.width
            iconsArea.anchors.topMargin = headerArea.height
            //listBox.width = 96 * 3
            moduleIconList.itemSize = 96
            removeHomeShutdownButton()
            rightBoxLoaderItem.iconId = ""
            moduleIconList.iconLabelOpacity = 1
            rightBox.x = panelContent.width
            showHomePanel.start()
        }
    }

    ParallelAnimation {
        id: showHomePanel
        PropertyAnimation {
            target: headerArea
            property: "y"
            to: 0
            duration: 200
        }

        PropertyAnimation {
            target: footerArea
            property: "anchors.bottomMargin"
            to: 0
            duration: 200
        }
        
        PropertyAnimation {
            target: iconsArea
            property: "opacity"
            to: 1
            duration: 200
        }
    }

    Loader {
        id: headerArea
        width: parent.width
        height: childrenRect.height
        active: true
        sourceComponent: HeaderArea {}
    }

    Rectangle{
        id: iconsArea
        anchors.top: parent.top
        anchors.topMargin: headerArea.height
        anchors.left: parent.left
        width: parent.width
        height: !isSiderNavigate ? parent.height - headerArea.height - footerArea.height : parent.height
        clip: true
        color: Qt.rgba(1, 1, 1, 0)

        GridView {
            id: moduleIconList
            width: !isSiderNavigate ? itemSize * 3 : itemSize
            height: parent.height
            anchors.horizontalCenter: parent.horizontalCenter
            cellHeight: itemSize
            cellWidth: itemSize

            property int itemSize: 96
            property real iconLabelOpacity: 1

            function iconClickAction(iconId) {
                currentContentId = iconId
                if (iconId == 'shutdown'){
                    shutdownButtonClicked()
                }
                else if(iconId == "home"){
                    trayIconTip.visible = false
                    toGridNavigateAnimation.start()
                    if (frame.x != rootWindow.width - panelWidth){
                        showAll.restart()
                    }
                }
                else{
                    if (frame.x != rootWindow.width - panelWidth){
                        showAll.restart()
                    }
                    if(!panelContent.isSiderNavigate){
                        toSiderNavigateAnimation.start()
                    }
                    else{
                        if(rightBoxLoaderItem.iconId != currentContentId){
                            rightBoxLoaderItem.iconId = currentContentId
                        }
                        else{
                            rootWindow.moduleIconClicked(currentContentId)
                        }
                    }
                }
            }

            function hightLightSelected(id){
                for(var i=0; i<navigateIconModel.count; i++){
                    if(navigateIconModel.get(i).moduleId == id){
                        moduleIconList.currentIndex = i
                    }
                }
            }

            delegate: ModuleIconItem {
                isSiderNavigate: panelContent.isSiderNavigate
            }
            model: navigateIconModel
            currentIndex: -1
        }
    }

    Item {
        id: footerArea
        width: parent.width
        height: shutdownButton.height + 36
        anchors.bottom: parent.bottom

        Behavior on anchors.bottomMargin {
            NumberAnimation { duration: 300 }
        }

        Image {
            id: bottomImage
            anchors.bottom: parent.bottom
            source: "images/shutdown_bg.png"
        }

        DImageButton {
            id: shutdownButton
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 36
            normal_image: "images/shutdown_normal.png"
            hover_image: "images/shutdown_hover.png"
            press_image: hover_image

            onStateChanged: {
                if(state == ""){
                    shutdownText.color = dconstants.fgColor
                } else {
                    shutdownText.color = tuhaoColor
                }
            }

            onClicked: {
                shutdownButtonClicked()
            }
        }

        DLabel {
            id: shutdownText
            text: dsTr("Power")
            anchors.top: shutdownButton.bottom
            anchors.topMargin: 6
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 12
        }
    }

    Rectangle {
        id: rightBox
        width: parent.width - trayWidth
        height: parent.height
        color: dconstants.bgColor
        x: panelContent.width
        clip: true

        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            //Eats mouse events
        }

        Row {
            id: rightBoxLoaderItem
            width: parent.width
            height: parent.height
            clip: true

            property string iconId
            onIconIdChanged: {
                rootWindow.moduleStartChange(iconId)
                rightBoxLoaderItem.visible = iconId != ''
                rightBoxLoader.iconId = iconId
                rightBoxLoader.source = (iconId == ''? '' : '../../modules/' + iconId + '/main.qml')
                if(iconId)
                    print("==> [info] load module:", iconId)
            }

            DSeparatorVertical {}

            Loader{
                id: rightBoxLoader
                property string iconId
                focus: true
                clip: true
                source: ''
                width: parent.width - 2
                height: parent.height

                onLoaded: {
                    if(iconId == "account" && headerArea.headerClicked){
                        timeoutShowCurrentUserDetail.start()
                    }
                }

                Timer {
                    id: timeoutShowCurrentUserDetail
                    interval: 200
                    running: false
                    repeat: false
                    onTriggered: {
                        rightBoxLoader.item.showCurrentUserDetail()
                        headerArea.headerClicked = false
                    }
                }
            }
        }
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

    Connections {
        target: rootWindow
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
}
