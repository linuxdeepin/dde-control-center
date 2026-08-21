// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0

import org.deepin.dcc 1.0

DccRepeater {
    enum WindowEffectType {
        Default = 0,
        Best,
        Better,
        Good,
        Normal,
        Compatible
    }

    delegate: DccObject {
        name: "effectItem_" + modelData.value
        parentName: "interfaceAndEffect"
        pageType: DccObject.Editor
        weight: 10 + index
        icon: modelData.icon
        displayName: modelData.title
        description: modelData.description
        backgroundType: DccObject.ClickStyle
        page: DccCheckIcon {
            visible: modelData.value === dccData.model.windowEffectType
        }
        onActive: function (cmd) {
            if (cmd === "") {
                dccData.worker.setWindowEffect(modelData.value)
            }
        }
    }
    model: [{
            "value": InterfaceEffectListview.WindowEffectType.Normal,
            "title": qsTr("Optimal Performance"),
            "icon": "optimum_performance",
            "description": qsTr("Disable all interface and window effects for efficient system performance.")
        }, {
            "value": InterfaceEffectListview.WindowEffectType.Better,
            "title": qsTr("Balance"),
            "icon": "balance",
            "description": qsTr("Limit some window effects for excellent visuals while maintaining smooth system performance.")
        }, {
            "value": InterfaceEffectListview.WindowEffectType.Best,
            "title": qsTr("Best Visuals"),
            "icon": "best_vision",
            "description": qsTr("Enable all interface and window effects for the best visual experience.")
        }]
}
