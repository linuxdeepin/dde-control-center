// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import org.deepin.dtk 1.0
import org.deepin.dcc 1.0


DccObject {
    id: userLicensePage
    name: "content"
    parentName: "system/userLicense"
    pageType: DccObject.Item
    weight: 20
    page: Label {
        Layout.alignment: Qt.AlignHCenter
        horizontalAlignment: Text.AlignLeft
        font: DTK.fontManager.t6
        text: dccData.systemInfoMode().userLicense
        wrapMode: Text.WordWrap
    }
}

