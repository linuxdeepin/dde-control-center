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
import Deepin.AppletWidgets 1.0

AppletPlugin {
    id: appletItem

    managed: true
    show: true
    name: dsTr("Network")
    iconPath: getIconUrl("network/small/wired_on.png")

    property var activeConnections: unmarshalJSON(dbusNetwork.activeConnections)
    property var activeConnectionsCount: {
        if (activeConnections)
            return  Object.keys(activeConnections).length
        else
            return 0
    }

    property bool hasWiredDevices: {
        if(nmDevices["wired"] && nmDevices["wired"].length > 0){
            return true
        }
        else{
            return false
        }
    }
    // wifi
    property bool hasWirelessDevices: {
        if(nmDevices["wireless"] && nmDevices["wireless"].length > 0){
            return true
        }
        else{
            return false
        }
    }

    Connections {
        target:root
        onDockDisplayModeChanged:{
            if (dockDisplayMode == 0){
                updateWiredSettingItem(true)
            }
            else{
                updateWiredSettingItem(false)
            }
        }
    }

    Timer {
        id:delayUpdateTimer
        repeat: false
        running: true
        interval: 1000
        onTriggered: {
            if (dockDisplayMode == 0){
                updateWiredSettingItem(true)
            }
            else{
                updateWiredSettingItem(false)
            }
        }
    }

    function updateWiredSettingItem(showFlag){
        for (var i = 0; i < appletInfos.count; i ++){
            if (appletInfos.get(i).applet_icon.indexOf("network") > 0){
                appletInfos.get(i).setting_enable = showFlag
            }
        }
    }


    appletTrayLoader: Loader {
        sourceComponent: AppletTray{}
        active: appletItem.show && ((hasWiredDevices && !hasWirelessDevices && activeConnectionsCount == 0 && dockDisplayMode != 0) || dockDisplayMode == 0)
    }

    onSubAppletStateChanged:{
        subAppletManager.updateAppletState(subAppletId, subAppletState)
    }

    SubAppletManager {
        id:subAppletManager
        parentAppletPath: appletItem.appletPath
    }
}
