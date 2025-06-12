// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import org.deepin.dtk 1.0 as D
import org.deepin.dcc 1.0

Window {
    id: root
    property string name: "screen"
    signal escPressed

    flags: Qt.CoverWindow | Qt.WindowStaysOnTopHint | Qt.SplashScreen | Qt.FramelessWindowHint | Qt.X11BypassWindowManagerHint
    D.DWindow.enabled: true
    x: screen.virtualX + (screen.width - width) * 0.5
    y: screen.virtualY + screen.height - height - 220
    width: control.implicitWidth + 44
    height: control.implicitHeight + 24
    minimumWidth: 200
    onClosing: destroy(10)
    Text {
        id: control
        anchors.centerIn: parent
        text: root.name
        font: D.DTK.fontManager.t4
        color: control.palette.brightText
    }
    Shortcut {
        sequence: "Esc"
        onActivated: escPressed()
        onActivatedAmbiguously: escPressed()
    }
}
