// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls

import org.deepin.dcc

DccObject {
    id: root
    name: "bluetooth"
    parentName: "device"
    displayName: qsTr("Bluetooth")
    description: qsTr("Bluetooth settings, devices")
    visible: false
    icon: "bluetoothNomal"
    weight: 10

    DccDBusInterface {
        id: bluetoothDbus
        service: "org.deepin.dde.Bluetooth1"
        path: "/org/deepin/dde/Bluetooth1"
        inter: "org.deepin.dde.Bluetooth1"
        connection: DccDBusInterface.SessionBus

        function errorSlot(adapters) {
            console.log("bluetooth GetAdapters errorSlot : ", adapters)
        }

        function getAdaptersSlot(adapters) {
            console.log(" bluetooth onGetAdapters", adapters)
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
            bluetoothDbus.callWithCallback("GetAdapters", [], getAdaptersSlot, errorSlot)
        }

        Component.onCompleted: {
            callWithCallback("GetAdapters", [], getAdaptersSlot, errorSlot)
        }
    }
}

