// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0

import org.deepin.dcc 1.0

DccObject {
    id: root
    name: "bluetooth"
    parentName: "device"
    displayName: "bluetooth"
    visible: false
    icon: "bluetoothNomal"
    weight: 70

    DccDBusInterface {
        id: bluetoothDbus
        service: "org.deepin.dde.Bluetooth1"
        path: "/org/deepin/dde/Bluetooth1"
        inter: "org.deepin.dde.Bluetooth1"
        connection: DccDBusInterface.SessionBus

        function errorSlot(adapters) {
            console.log("bluetooth GetAdapters errorSlot : ",adapters)
        }

        function onGetAdapters(adapters) {
            console.log(" bluetooth onGetAdapters",adapters)
            if (adapters === "[]") {
                 root.visible = false
            } else {
                root.visible = true
            }
        }

        function onAdapterAdded(adapters) {
            root.visible = true
        }
        function onAdapterRemoved(adapters) {
            bluetoothDbus.callWithCallback("GetAdapters", "", onGetAdapters, errorSlot)
        }

        Component.onCompleted: {
            callWithCallback("GetAdapters", "", onGetAdapters, errorSlot)
            connectSignal("AdapterAdded",onAdapterAdded)
            connectSignal("AdapterRemoved",onAdapterRemoved)
        }
    }
}

