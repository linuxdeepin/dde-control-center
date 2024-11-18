// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.0

import org.deepin.dcc 1.0
import org.deepin.dcc.defApp 1.0

DccObject {
    DetailItem {
        name: "defappWebpage"
        parentName: "defaultapp"
        displayName: qsTr("Webpage")
        icon: "dcc_network"
        weight: 10
        categoryModel: dccData.browser()
    }
    DetailItem {
        name: "defappMail"
        parentName: "defaultapp"
        displayName: qsTr("Mail")
        icon: "dcc_mail"
        weight: 20
        categoryModel: dccData.mail()
    }
    DetailItem {
        name: "defappText"
        parentName: "defaultapp"
        displayName: qsTr("Text")
        icon: "dcc_text"
        weight: 30
        categoryModel: dccData.text()
    }
    DetailItem {
        name: "defappMusic"
        parentName: "defaultapp"
        displayName: qsTr("Music")
        icon: "dcc_music"
        weight: 40
        categoryModel: dccData.music()
    }
    DetailItem {
        name: "defappVideo"
        parentName: "defaultapp"
        displayName: qsTr("Video")
        icon: "dcc_video"
        weight: 50
        categoryModel: dccData.video()
    }
    DetailItem {
        name: "defappPicture"
        parentName: "defaultapp"
        displayName: qsTr("Picture")
        icon: "dcc_picture"
        weight: 60
        categoryModel: dccData.picture()
    }
    DetailItem {
        name: "defappTerminal"
        parentName: "defaultapp"
        displayName: qsTr("Terminal")
        icon: "dcc_terminal"
        weight: 70
        categoryModel: dccData.terminal()
    }
}
