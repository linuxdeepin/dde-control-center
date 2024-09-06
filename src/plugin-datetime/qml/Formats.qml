// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

// 时间格式、货币格式等
DccObject {
    DccObject {
        id: fmt
        name: "formatContent"
        parentName: "format"
        displayName: qsTr("format")
        icon: "dcc_nav_datetime"
        weight: 24
    }
}
