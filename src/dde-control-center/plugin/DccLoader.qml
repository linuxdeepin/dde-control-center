// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick

Loader {
    property var dccObj: null
    property Item dccObjItem: null

    function updateDccObjItem() {
        if (dccObj) {
            dccObj.parentItem = dccObjItem
        }
    }
    enabled: dccObj && dccObj.enabledToApp
    // asynchronous: true
    sourceComponent: dccObj ? dccObj.page : null
    onDccObjChanged: updateDccObjItem()
    onDccObjItemChanged: updateDccObjItem()
    Component.onCompleted: updateDccObjItem()
    onStatusChanged: {
        if (status === Loader.Error) {
            console.warn("Failed to load component for dccObj:", dccObj)
        }
    }
}
