// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import org.deepin.dtk 1.0
import org.deepin.dcc 1.0
//import org.deepin.dcc.systemInfo 1.0

DccObject {

    DccObject {
        name: "title"
        parentName: "system/privacyPolicy"
        pageType: DccObject.Item
        weight: 20
        page: Label {
                Layout.alignment: Qt.AlignHCenter
                horizontalAlignment: Text.AlignHCenter
                font: DTK.fontManager.t4
                text: qsTr("Privacy Policy")
            }
    }
    DccObject {
        name: "content"
        parentName: "system/privacyPolicy"
        pageType: DccObject.Item
        weight: 30
        page: Label {
                font: DTK.fontManager.t6
                horizontalAlignment: Text.AlignLeft
                text: dccData.systemInfoMode().privacyPolicy
                wrapMode: Text.WordWrap
                opacity: 0.7
        }
    }
}
