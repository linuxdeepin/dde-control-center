// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3

import Dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    DccObject {
        name:"defappWebpage"
        parentName:"defaultapp"
        displayName: qsTr("Webpage")
        icon:"dcc_browser"
        weight: 10
    }
    DccObject {
        name:"defappMail"
        parentName:"defaultapp"
        displayName: qsTr("Mail")
        icon:"dcc_mail"
        weight: 20
    }
    DccObject {
        name:"defappText"
        parentName:"defaultapp"
        displayName: qsTr("Text")
        icon:"dcc_text"
        weight: 30
    }
    DccObject {
        name:"defappMusic"
        parentName:"defaultapp"
        displayName: qsTr("Music")
        icon:"dcc_music"
        weight: 40
    }
    DccObject {
        name:"defappVideo"
        parentName:"defaultapp"
        displayName: qsTr("Video")
        icon:"dcc_video"
        weight: 50
    }
    DccObject {
        name:"defappPicture"
        parentName:"defaultapp"
        displayName: qsTr("Picture")
        icon:"dcc_photo"
        weight: 60
    }
    DccObject {
        name:"defappTerminal"
        parentName:"defaultapp"
        displayName: qsTr("Terminal")
        icon:"dcc_terminal"
        weight: 70
    }
}




