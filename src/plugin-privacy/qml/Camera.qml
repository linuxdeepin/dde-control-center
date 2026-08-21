// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

import org.deepin.dcc.privacy 1.0

DccObject {
    DccObject {
        name: "cameraAppViewGroup"
        parentName: "privacy/camera"
        weight: 100
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "title"
            weight: 1
            parentName: "privacy/camera/cameraAppViewGroup"
            pageType: DccObject.Editor
            canSearch: false
            displayName: qsTr("Allow below apps to access your camera:")
        }

        DccRepeater {
            model: dccData.appsModel
            delegate: DccObject {
                name: "plugin" + model.itemKey
                parentName: "privacy/camera/cameraAppViewGroup"
                weight: 10 + index * 10
                icon: model.iconName
                displayName: model.name
                pageType: DccObject.Editor
                canSearch: false
                backgroundType: DccObject.Hover
                page: D.Switch {
                    checked: model.cameraPermission
                    onCheckedChanged: {
                        if (checked !== model.cameraPermission) {
                            dccData.worker.setPremissionEnabled(index, ApplicationItem.CameraPermission, checked)
                        }
                    }
                }
            }
        }
    }
}
