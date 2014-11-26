/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Wanqing Yang
**
**  Author:     Wanqing Yang <yangwanqing@linuxdeepin.com>
**  Maintainer: Wanqing Yang <yangwanqing@linuxdeepin.com>
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
import DBus.Com.Deepin.Daemon.Bluetooth 1.0

AppletPlugin {
    id: appletItem

    managed: true
    show: true
    name:  appletName == "" ? appletId : appletName
    iconPath:getIconUrl("bluetooth/bluetooth-enable.png")

    Bluetooth {
        id: dbus_bluetooth
        onAdapterAdded:bluetoothAdapters = unmarshalJSON(dbusBluetooth.GetAdapters())
        onAdapterRemoved:bluetoothAdapters = unmarshalJSON(dbusBluetooth.GetAdapters())
        onAdapterPropertiesChanged: bluetoothAdapters = unmarshalJSON(dbusBluetooth.GetAdapters())
    }
    property var bluetoothAdapters: unmarshalJSON(dbusBluetooth.GetAdapters())
    property var blueToothAdaptersCount: {
        if (bluetoothAdapters)
            return bluetoothAdapters.length
        else
            return 0
    }

    property var dockMode: dockDisplayMode

    appletTrayLoader: Loader {
        sourceComponent: AppletTray{}
        active:blueToothAdaptersCount > 0 && appletItem.show && dockMode != 0//not mac mode
    }
}
