// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import org.deepin.dcc 1.0

DccObject {
    DccObject {
        name: "datetime"
        parentName: "system"
        displayName: qsTr("Time and date")
        description: qsTr("Time and date, time zone settings")
        icon: "dcc_time_date"
        weight: 40
    }
    DccObject {
        id: langAndFormat
        name: "langAndFormat"
        parentName: "system"
        displayName: qsTr("Language and region")
        description: qsTr("System language, regional formats")
        icon: "dcc_lang_format"
        weight: 45
        visible: false
        DccDBusInterface {
            property var locales
            service: "org.deepin.dde.LangSelector1"
            path: "/org/deepin/dde/LangSelector1"
            inter: "org.deepin.dde.LangSelector1"
            connection: DccDBusInterface.SessionBus
            onLocalesChanged: langAndFormat.visible = true
        }
    }
}


