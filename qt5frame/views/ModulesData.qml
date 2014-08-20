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
    id: moduleData

    function getMaxModuleNameWidth(){
        var max = 0
        for(var i in allIds){
            var t = moduleLocaleNames[allIds[i]]
            var tmpText = iconTipTextComponent.createObject(moduleData, { text: t})
            var width = Math.ceil(tmpText.width)
            if (max < width){
                max = width
            }
        }
        return max + 60
    }

    Component {
        id: iconTipTextComponent
        Text{
            font.pixelSize: 13
            color: "white"
            visible: false
        }
    }

    property var moduleLocaleNames: {
            "date_time": dsTr("Date and Time"),
            "network": dsTr("Network"),
            "disk_mount": dsTr("Disk Mount"),
            "bluetooth": dsTr("Bluetooth"),
            "sound": dsTr("Sound"),
            "power": dsTr("Power Management"),
            "dss": dsTr("More Settings"),
            "shutdown": dsTr("Power"),
            "display": dsTr("Display"),
            "desktop": dsTr("Desktop"),
            "personalization": dsTr("Personalization"),
            "shortcuts": dsTr("Keyboard Shortcuts"),
            "keyboard": dsTr("Keyboard and Language"),
            "mouse_touchpad": dsTr("Mouse and Touchpad"),
            "mouse": dsTr("Mouse"),
            "account": dsTr("User Accounts"),
            "default_applications": dsTr("Default Applications"),
            "system_info": dsTr("System Information"),
            "grub": dsTr("Boot Menu"),
            "home": dsTr("Home")
        }

    property var allIds: [
            "account",
            "display",
            "default_applications",
            "personalization",
            "network",
            "bluetooth",
            "sound",
            "date_time",
            "power",
            "mouse_touchpad",
            "keyboard",
            "shortcuts",
            "grub",
            "system_info"
        ]
}
