// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.0
import QtQuick.Controls 2.0

import org.deepin.dtk 1.0 as D

import org.deepin.dcc 1.0

DccObject {
    id: root
    name: "system"
    parentName: "root"
    displayName: qsTr("system")
    icon: "commoninfo"
    weight: 20

    DccObject {
        name: "common"
        parentName: "system"
        displayName: qsTr("Common settings")
        weight: 5
        pageType: DccObject.Item
        page: Item {
            implicitHeight: label.implicitHeight + label.anchors.topMargin
            Label {
                id: label
                anchors {
                    left: parent.left
                    top: parent.top
                    leftMargin: 10
                    topMargin: 10
                }
                font: DccUtils.copyFont(D.DTK.fontManager.t4, {
                                            "bold": true
                                        })
                text: dccObj.displayName
            }
        }
    }
}
