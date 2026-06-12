// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import org.deepin.dtk 1.0 as D
import org.deepin.ds 1.0 as DS
import org.deepin.dcc 1.0

Window {
    id: root
    property string name: "screen"
    signal escPressed

    flags: Qt.CoverWindow | Qt.WindowStaysOnTopHint | Qt.SplashScreen | Qt.FramelessWindowHint | Qt.X11BypassWindowManagerHint
    D.DWindow.enabled: true
    color: root.palette.window
    // 使用 DLayerShellWindow 定位窗口：X轴居中，Y轴锚定底部偏上1/4处
    DS.DLayerShellWindow.anchors: DS.DLayerShellWindow.AnchorBottom
    DS.DLayerShellWindow.layer: DS.DLayerShellWindow.LayerOverlay
    DS.DLayerShellWindow.bottomMargin: Math.max(0, screen.height * 0.25 - height * 0.5)

    width: control.implicitWidth
    height: control.implicitHeight
    minimumWidth: 200
    onClosing: destroy(10)
    Text {
        id: control
        leftPadding: 22
        topPadding: 12
        rightPadding: leftPadding
        bottomPadding: topPadding
        text: root.name
        font: D.DTK.fontManager.t4
        color: root.palette.brightText
    }
    Shortcut {
        sequence: "Esc"
        onActivated: escPressed()
        onActivatedAmbiguously: escPressed()
    }
}
