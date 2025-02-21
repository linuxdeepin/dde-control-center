//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    DccObject {
        name: "pluginAreaTitle"
        weight: 10
        parentName: "personalization/dock/pluginArea"
        pageType: DccObject.Item
        displayName: qsTr("Plugin Area")
        description: qsTr("Select which icons appear in the Dock")
        page: ColumnLayout {
            Label {
                font.family: D.DTK.fontManager.t4.family
                font.bold: true
                font.pixelSize: D.DTK.fontManager.t4.pixelSize
                text: dccObj.displayName
            }
            Label {
                text: dccObj.description
            }
        }
    }

    DccObject {
        name: "pluginAreaView"
        parentName: "personalization/dock/pluginArea"
        weight: 100
        pageType: DccObject.Item
        page: DccGroupView {}

        DccRepeater {
            model: dccData.pluginModel
            delegate: DccObject {
                name: "plugin" + model.key
                property real iconSize: 16
                parentName: "personalization/dock/pluginArea/pluginAreaView"
                weight: 10 + index * 10
                backgroundType: DccObject.Normal
                icon: model.icon
                displayName: model.displayName
                pageType: DccObject.Editor
                page: DccCheckIcon {
                    checked: model.visible
                    onClicked: {
                        dccData.dockInter.setItemOnDock(model.settingKey, model.key, !checked)
                    }
                }
            }
        }
    }
}
