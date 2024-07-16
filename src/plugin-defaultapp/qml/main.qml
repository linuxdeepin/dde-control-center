
// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import org.deepin.dcc 1.0
import DefApp 1.0

DccObject {
    DccObject {
        id: defappWebpage
        name: "defappWebpage"
        parentName: "defaultapp"
        displayName: qsTr("Webpage")
        icon: "dcc_browser"
        weight: 10
        page: DccGroupView {}
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
        page: DccGroupView {}
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
        page: DccGroupView {}
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
        page: DccGroupView {}
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
        page: DccGroupView {}
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
        icon: "dcc_photo"
        weight: 60
        page: DccGroupView {}
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
        page: DccGroupView {}
        DetailItem {
            parentObj: defappTerminal
            categoryModel: dccData.terminal()
        }
    }
}
