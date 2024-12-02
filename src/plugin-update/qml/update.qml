// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.0
import QtQuick.Controls 2.0
import Qt.labs.qmlmodels 1.2
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D

import org.deepin.dcc 1.0

DccObject {
    name: "update"
    parentName: "root"
    displayName: qsTr("System Update")
    description: qsTr("System update and upgrade")
    icon: "update"
    weight: 100
}
