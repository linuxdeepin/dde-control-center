// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.0

import org.deepin.dcc 1.0
import org.deepin.dcc.defApp 1.0

DccObject {
    Component {
        id: groupView
        DccRightView {
            isGroup: true
        }
    }
    DccObject {
        id: defappWebpage
        name: "defappWebpage"
        parentName: "defaultapp"
        displayName: qsTr("Webpage")
        icon: "dcc_network"
        weight: 10
        page: groupView
        DetailItem {
            parentObj: defappWebpage
            categoryModel: dccData.browser()
        }
    }
    DccObject {
        id: defappMail
        name: "defappMail"
        parentName: "defaultapp"
        displayName: qsTr("Mail")
        icon: "dcc_mail"
        weight: 20
        page: groupView
        DetailItem {
            parentObj: defappMail
            categoryModel: dccData.mail()
        }
    }
    DccObject {
        id: defappText
        name: "defappText"
        parentName: "defaultapp"
        displayName: qsTr("Text")
        icon: "dcc_text"
        weight: 30
        page: groupView
        DetailItem {
            parentObj: defappText
            categoryModel: dccData.text()
        }
    }
    DccObject {
        id: defappMusic
        name: "defappMusic"
        parentName: "defaultapp"
        displayName: qsTr("Music")
        icon: "dcc_music"
        weight: 40
        page: groupView
        DetailItem {
            parentObj: defappMusic
            categoryModel: dccData.music()
        }
    }
    DccObject {
        id: defappVideo
        name: "defappVideo"
        parentName: "defaultapp"
        displayName: qsTr("Video")
        icon: "dcc_video"
        weight: 50
        page: groupView
        DetailItem {
            parentObj: defappVideo
            categoryModel: dccData.video()
        }
    }
    DccObject {
        id: defappPicture
        name: "defappPicture"
        parentName: "defaultapp"
        displayName: qsTr("Picture")
        icon: "dcc_picture"
        weight: 60
        page: groupView
        DetailItem {
            parentObj: defappPicture
            categoryModel: dccData.picture()
        }
    }
    DccObject {
        id: defappTerminal
        name: "defappTerminal"
        parentName: "defaultapp"
        displayName: qsTr("Terminal")
        icon: "dcc_terminal"
        weight: 70
        page: groupView
        DetailItem {
            parentObj: defappTerminal
            categoryModel: dccData.terminal()
        }
    }
}
