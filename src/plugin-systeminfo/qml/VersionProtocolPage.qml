// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

import org.deepin.dtk
import org.deepin.dcc

DccObject {
    DccObject {
        id: versionProtocolPage
        name: "title"
        parentName: "system/versionProtocol"
        pageType: DccObject.Item
        weight: 20
        page: Label {
                Layout.alignment: Qt.AlignHCenter
                horizontalAlignment: Text.AlignHCenter
                font: DTK.fontManager.t4
                text: dccData.systemInfoMode().gnuLinceseTitle
            }
    }
    DccObject {
        id: versionProtocolPageContent
        name: "content"
        parentName: "system/versionProtocol"
        pageType: DccObject.Item
        weight: 30
        page: Label {
            Layout.alignment: Qt.AlignVCenter
            font: DTK.fontManager.t6
            horizontalAlignment: Text.AlignLeft
            text: dccData.systemInfoMode().gnuLinceseContent
            wrapMode: Text.WordWrap
            opacity: 0.7
        }
    }
}
