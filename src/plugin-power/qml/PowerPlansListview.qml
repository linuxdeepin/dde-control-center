// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.deepin.dtk as D
import org.deepin.dcc

DccRepeater {
    id: repeater
    delegate: DccObject {
        visible: {
            if (modelData.mode === "performance") {
                return dccData.model.isHighPerformanceSupported
            } else if (modelData.mode === "balance_performance") {
                return dccData.model.isBalancePerformanceSupported
            }
            return true
        }
        name: "powerPlans" + index
        parentName: "powerPlans"
        pageType: DccObject.Editor
        weight: 10 + index
        icon: modelData.icon
        displayName: modelData.title
        description: modelData.description
        // pageType: DccObject.Item
        backgroundType: DccObject.ClickStyle
        page: DccCheckIcon {
            visible: modelData.mode === dccData.model.powerPlan
        }
        onActive: function (cmd) {
            if (cmd === "") {
                dccData.worker.setPowerPlan(modelData.mode)
            }
        }
    }
    model: [{
            "mode": "performance",
            "title": qsTr("High Performance"),
            "icon": "high_performance",
            "description": qsTr("Prioritize performance, which will significantly increase power consumption and heat generation")
        }, {
            "mode": "balance_performance",
            "title": qsTr("Balance Performance"),
            "icon": "balance_performance",
            "description": qsTr("Aggressively adjust CPU operating frequency based on CPU load condition")
        }, {
            "mode": "balance",
            "title": qsTr("Balanced"),
            "icon": "balanced",
            "description": qsTr("Balancing performance and battery life, automatically adjusted according to usage")
        }, {
            "mode": "powersave",
            "title": qsTr("Power Saver"),
            "icon": "power_performance",
            "description": qsTr("Prioritize battery life, which the system will sacrifice some performance to reduce power consumption")
        }]
}
