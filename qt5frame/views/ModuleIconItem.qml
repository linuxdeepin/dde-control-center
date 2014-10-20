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
import Deepin.Widgets 1.0

Rectangle {
    id: moduleIconItem
    clip: true

    width: GridView.view.itemSize
    height: GridView.view.itemSize
    color: Qt.rgba(1, 1, 1, 0)

    property bool isSelected: rightBoxLoaderItem.iconId == itemId
    property bool isSiderNavigate: false

    onIsSelectedChanged: {
        if(isSelected){
            state = "selected"
        }
        else{
            state = ""
        }
    }

    property string itemId: moduleId
    property string itemLabel: moduleLocaleName
    property string iconPathHeader: isSiderNavigate ? "small_icons/" + moduleId : "icons/" + moduleId
    property bool hover: false
    //property url iconPath: windowView.isIconPluginExist(moduleId) ? '../../modules/' + moduleId + '/iconPlugin.qml' : ''
    property url iconPath: ""

    QtObject {
        id: defaultIcon
        property url normalImage: iconPathHeader + '_normal.png'
        property url hoverImage: iconPathHeader + '_hover.png'
        property url pressImage: iconPathHeader + '_press.png'
    }

    Loader {
        id: pluginLoader
        source: iconPath
        property bool showWarning: false
        property int tipNumber: 0
        property var icon: defaultIcon
    }

    states: [
        State {
            name: "hovered"
            PropertyChanges { target: moduleIcon; source: pluginLoader.icon.hoverImage }
            PropertyChanges { target: moduleName; color: "#FFF" }
        },
        State {
            name: "selected"
            PropertyChanges { target: moduleIcon; source: pluginLoader.icon.pressImage; anchors.topMargin: 8 }
            PropertyChanges { target: moduleName; visible: false }
        }
    ]

    function changeState(s){
        if(moduleIconItem.state != "selected"){
            moduleIconItem.state = s
        }
    }

    Item {
        property int size: parent.width - 6
        width: size
        height: size
        anchors.centerIn: parent

        Rectangle {
            id: outBox
            anchors.fill: parent
            radius: 3
            color: dconstants.contentBgColor
            visible: {
                if(isSiderNavigate){
                    return false
                }
                else{
                    return moduleIconItem.hover
                }
            }
        }

        Image {
            id: moduleIcon
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: moduleName.visible ? -16 : 0
            source: pluginLoader.icon.normalImage
        }

        DLabel {
            id: moduleName
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: moduleIcon.bottom
            anchors.topMargin: 6
            width: parent.width - 4
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12
            text: itemLabel
            elide: Text.ElideRight
            visible: opacity != 0
            opacity: moduleIconItem.GridView.view.iconLabelOpacity
        }

        Image {
            id: warning
            anchors.top: parent.top
            anchors.topMargin: 2
            anchors.left: parent.left
            anchors.leftMargin: 2
            source: "images/warning.svg"
            visible: pluginLoader.showWarning
        }

        NumberTip {
            id: numberTip
            anchors.top: parent.top
            anchors.topMargin: 2
            anchors.left: parent.left
            anchors.leftMargin: 2
            visible: pluginLoader.tipNumber != 0
            currentNumber: pluginLoader.tipNumber
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                moduleIconItem.GridView.view.iconClickAction(moduleIconItem.itemId)
            }
            onEntered: {
                changeState("hovered")
                moduleIconItem.hover = true
            }
            onExited: {
                changeState("")
                moduleIconItem.hover = false
            }
            onPressed: {
            }
            onReleased: {
                if(containsMouse){
                    changeState("hovered")
                    moduleIconItem.hover = true
                }
                else{
                    changeState("")
                    moduleIconItem.hover = false
                }
            }
        }

    }

    onHoverChanged: {
        panelContent.trayIconHoverHandler(moduleId, index, hover)
    }
}
