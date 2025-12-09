// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick

Loader {
    property var dccObj: null

    enabled: !dccObj || dccObj.enabledToApp
    // asynchronous: true
    onDccObjChanged: {
        if (dccObj && dccObj.page) {
            if (sourceComponent === dccObj.page) {
                sourceComponent = null
            }
            sourceComponent = dccObj.page
        } else {
            sourceComponent = null
        }
    }
    onStatusChanged: {
        if (status === Loader.Error) {
            console.warn("Failed to load component for dccObj:", dccObj)
            sourceComponent = null
        }
    }
    Connections {
        target: dccObj
        function onPageChanged() {
            sourceComponent = dccObj.page
        }
    }
}
