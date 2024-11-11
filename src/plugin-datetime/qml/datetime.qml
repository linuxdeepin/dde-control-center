// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import org.deepin.dcc 1.0

DccObject {
    id: datetime
    // 时间和日期
    DccObject {
        name: "timeAndLang"
        parentName: "system"
        displayName: qsTr("Time and date")
        description: qsTr("Set your time and date")
        icon: "dcc_time_date"
        weight: 40
    }

    // 语言和格式
    DccObject {
        name: "langAndFormat"
        parentName: "system"
        displayName: qsTr("Language and format")
        description: qsTr("Set your system language and region formats")
        icon: "dcc_lang_format"
        weight: 45
    }

    visible: false
    DccDBusInterface {
        service: "org.deepin.dde.LangSelector1"
        path: "/org/deepin/dde/LangSelector1"
        inter: "org.deepin.dde.LangSelector1"
        connection: DccDBusInterface.SessionBus
        monitorProperties: ["Locales"]
        onPropertyChanged: function (properties) {
            datetime.visible = true
        }
    }
}
