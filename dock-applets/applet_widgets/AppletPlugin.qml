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

Item {
    id: appletItem
    width: 1
    height: 1

    property bool managed: true
    property bool show: true
    property string name: ""
    property string iconPath:""
    property string appletPath:""//for sub applet

    signal subAppletStateChanged(string subAppletId,bool subAppletState)

    function setAppletPath(applet_path){
        appletItem.appletPath = applet_path
    }

    function setAppletState(applet_visible) {
        if(managed){
            show = applet_visible
        }
    }

    function setSubAppletState(applet_id,new_state){
        appletItem.subAppletStateChanged(applet_id,new_state)
    }

    property var appletTrayLoader: Loader {}

    Component.onCompleted: {
        if(managed){
            show = root.getInitAppletSate(appletId)
            appletInfos.update(appletId, name, show,iconPath)
        }
    }
}
