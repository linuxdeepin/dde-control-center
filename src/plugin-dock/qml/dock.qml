//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    name: "dock"
    parentName: "personalization"
    displayName: qsTr("Desktop and taskbar")
    description: qsTr("Desktop organization, taskbar mode, plugin area settings")
    icon: "dock"
    weight: 100
}
