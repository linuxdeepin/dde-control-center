// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import org.deepin.dcc 1.0

Window {
    id: root
    flags: Qt.CoverWindow | Qt.WindowStaysOnTopHint | Qt.SplashScreen | Qt.FramelessWindowHint | Qt.X11BypassWindowManagerHint
    color: "#2ca7f8"
    x: screen.virtualX
    y: screen.virtualY
    width: screen.width
    height: 10
    onClosing: destroy(10)
    Timer {
        interval: 1000
        running: root.visible
        onTriggered: root.close()
    }
    Window {
        flags: Qt.CoverWindow | Qt.WindowStaysOnTopHint | Qt.SplashScreen | Qt.FramelessWindowHint | Qt.X11BypassWindowManagerHint
        visible: root.visible
        color: root.color
        screen: root.screen
        x: screen.virtualX
        y: screen.virtualY + screen.height - 10
        width: screen.width
        height: 10
    }
    Window {
        flags: Qt.CoverWindow | Qt.WindowStaysOnTopHint | Qt.SplashScreen | Qt.FramelessWindowHint | Qt.X11BypassWindowManagerHint
        visible: root.visible
        color: root.color
        screen: root.screen
        x: screen.virtualX
        y: screen.virtualY
        width: 10
        height: screen.height
    }
    Window {
        flags: Qt.CoverWindow | Qt.WindowStaysOnTopHint | Qt.SplashScreen | Qt.FramelessWindowHint | Qt.X11BypassWindowManagerHint
        visible: root.visible
        color: root.color
        screen: root.screen
        x: screen.virtualX + screen.width - 10
        y: screen.virtualY
        width: 10
        height: screen.height
    }
}
