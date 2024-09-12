// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    DccObject {
        name: "general"
        parentName: "power"
        displayName: qsTr("General")
        description: "Performance mode switching, energy-saving settings, wake-up settings, shutdown settings"
        icon: "general"
        weight: 10
        page: DccRightView {
            spacing: 5
        }
        GeneralPage {}
    }
    DccObject {
        name: "onPower"
        parentName: "power"
        displayName: qsTr("Plugged In")
        description: qsTr("Screen and standby management")
        icon: "plugged_in"
        weight: 100
        page: DccRightView {
            spacing: 5
        }
        PowerPage {}
    }
    DccObject {
        name: "onBattery"
        parentName: "power"
        displayName: qsTr("On Battery")
        description: qsTr("Screen and standby management, low battery management, battery management")
        icon: "on_battery"
        weight: 200
        visible: dccData.model.haveBettary
        page: DccRightView {
            spacing: 5
        }
        BatteryPage {}
    }
}
