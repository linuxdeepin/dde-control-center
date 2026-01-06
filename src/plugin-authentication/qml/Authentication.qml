// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import org.deepin.dcc 1.0
import QtQuick 2.15
import QtQuick.Controls 2.15

DccObject {
    id: authentication
    property bool hasFingerprint: false
    property bool hasChara: false
    name: "authentication"
    parentName: "accountsloginMethod"
    displayName: qsTr("Biometric Authentication")
    weight: 30
    visible: (hasFingerprint || hasChara) && !DccApp.isTreeland()


    DccDBusInterface {
        property var driverInfo
        service: "org.deepin.dde.Authenticate1"
        path: "/org/deepin/dde/Authenticate1/CharaManger"
        inter: "org.deepin.dde.Authenticate1.CharaManger"
        connection: DccDBusInterface.SystemBus
        onDriverInfoChanged: {
            let jsonData = JSON.parse(driverInfo)
            let found = false
            for (var i = 0; i < jsonData.length; i++) {
                if (jsonData[i].CharaType !== 0) {
                    authentication.hasChara = true
                    return
                }
            }
            authentication.hasChara = found
        }
    }

    DccDBusInterface {
        property var defaultDevice
        service: "org.deepin.dde.Authenticate1"
        path: "/org/deepin/dde/Authenticate1/Fingerprint"
        inter: "org.deepin.dde.Authenticate1.Fingerprint"
        connection: DccDBusInterface.SystemBus
        onDefaultDeviceChanged: {
            authentication.hasFingerprint = defaultDevice !== ""
        }
    }
}
