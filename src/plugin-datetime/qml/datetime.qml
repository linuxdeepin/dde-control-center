// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import org.deepin.dcc 1.0

DccObject {
    // 时间和日期
    DccObject {
        name: "timeAndLang"
        parentName: "system"
        displayName: qsTr("Time and language")
        description: qsTr("Set your time and date")
        icon: "dcc_time_date"
        weight: 40
    }

    // 语言和格式
    DccObject {
        name: "langAndFormat"
        parentName: "system"
        displayName: qsTr("Language and format")
        description: qsTr("Set your system language, keyboard layout, and input method")
        icon: "dcc_lang_format"
        weight: 45
    }
}
