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

import QtQuick 2.0
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import "../widgets"

DockApplet{
    title: "Disk Mount"
    appid: "AppletDiskMount"
    icon: dockDisplayMode == 0 ? getIconUrl("disk_mount/icon.png") : getIconUrl("disk_mount/icon_16.png")
    property int windowWidth: 224
    property int xEdgePadding: 12

    onActivate:{
        //showDiskMount(0)
    }

    onNativeWindowDestroyed: {
        toggle_applet("disk_mount")
        toggle_applet("disk_mount")
    }

    onQt5ScreenDestroyed: {
        console.log("Recive onQt5ScreenDestroyed")
        mainObject.restartDockApplet()
    }

    function showDiskMount(id){
        dbusControlCenter.ShowModule("default_applications")
    }

    function hideDiskMount(id){
        set_hide_applet("disk_mount")
    }

    //menu: Menu{
        //Component.onCompleted: {
            //addItem(dsTr("_Run"), showDiskMount);
        //}
    //}

    function bitToHuman(b){
        b = b*1000
        var factor = 1000
        if (b < factor) {
            return b + " B"
        }
        else{
            b = b/factor
            if ( b < factor ){
                return b.toFixed(2) +" KB"
            }
            else {
                b = b/factor
                if (b < factor){
                    return b.toFixed(2) + " MB"
                }
                else{
                    b = b/factor
                    if(b<10){
                        return b.toFixed(2) + " GB"
                    }
                    else if(b<100){
                        return b.toFixed(1) + " GB"
                    }
                    else{
                        return parseInt(b) + " GB"
                    }
                }
            }
        }
    }

    window: DockQuickWindow {
        id: root
        width: windowWidth
        height: diskListView.height + xEdgePadding * 2
        color: "transparent"

        ListView {
            id: diskListView
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: xEdgePadding
            width: windowWidth
            height: Math.min(68 * mountDiskList.length, 300)
            model: mountDiskList
            clip: true
            delegate: DiskItem {
                width: windowWidth
            }

            DScrollBar {
                flickable: parent
            }
        }
    }
}
