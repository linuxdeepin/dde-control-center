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
import QtQuick.Window 2.1

Loader {
    id: appletRootLoader
    property url qmlPath: applet_path
    property string appletId: applet_id
    source: qmlPath

    function setAppletState(newState){
        if(isItemValid()){
            item.setAppletState(newState)
        }
    }

    function toggleAppletState(){
        if(isItemValid()){
            item.setAppletState(!item.show)
        }
    }

    function isItemValid(){
        return typeof(appletRootLoader.item) != "undefined"
    }

    Connections {
        target: appletRootLoader.item
        onShowChanged: {
            if(appletRootLoader.isItemValid() && appletRootLoader.item.managed){
                appletInfos.update(appletId, appletRootLoader.item.name, appletRootLoader.item.show, appletRootLoader.item.iconPath)
            }
        }
    }

    Connections {
        target: root
        onDockDisplayModeChanged: {
            if(appletRootLoader.isItemValid() && appletRootLoader.item.managed){
                var applet_visible = root.getInitAppletSate(appletRootLoader.appletId)
                appletRootLoader.setAppletState(applet_visible)
            }
        }
    }
}
