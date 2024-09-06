// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

// import QtQuick 2.15
import org.deepin.dcc 1.0

DccObject {
    DccObject {
        name: "dateTime"
        parentName: "timeAndLang"
        displayName: qsTr("Time and date")
        description: qsTr("Time and timezone settings")
        icon: "dcc_nav_datetime"
        weight: 11

        TimeAndDate { }
    }

    DccObject {
        name: "langAndRegion"
        parentName: "timeAndLang"
        displayName: qsTr("Language and region")
        description: qsTr("Set your system language, keyboard layout, and input method")
        icon: "dcc_nav_datetime"
        weight: 21
        LangAndRegion { }
    }

    DccObject {
        name: "format"
        parentName: "timeAndLang"
        displayName: qsTr("Format")
        description: qsTr("Time format, monetary format, number format")
        icon: "dcc_nav_datetime"
        weight: 31
    }
}
