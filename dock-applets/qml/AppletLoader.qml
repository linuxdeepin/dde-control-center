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
    property url qmlPath: "%1/main.qml".arg(applet_id)
    property string appletId: applet_id

    function show(){
        source = qmlPath
    }

    function hide(){
        source = ""
    }

    function toggle(){
        if(source == ""){
            source = qmlPath
        }
        else{
            source = ""
        }
    }

    Component.onCompleted: {
        if(lastStateInfos == "[]" || lastStateInfos == "" || applet_id == "date_time" || applet_id == "disk_mount"){
            show()
        }
        else {
            var infos = unmarshalJSON(lastStateInfos)
            for(var i in infos){
                var info = infos[i]
                if(info[0] == applet_id && info[2]){
                    show()
                }
            }
        }
    }
}
