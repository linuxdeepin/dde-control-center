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
import DBus.Com.Deepin.Daemon.Power 1.0
import Deepin.AppletWidgets 1.0

AppletPlugin {
    id: appletItem

    managed: dbusPower.batteryIsPresent
    show: managed && true
    name: dsTr("Power")
    iconPath: "dock-battery-charge"

    // Power
    property var dbusPower: Power{}

    appletTrayLoader: Loader {
        sourceComponent: AppletTray{}
        active: appletItem.show
    }

}
