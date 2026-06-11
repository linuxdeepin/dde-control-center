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

    flags: Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint
    D.DWindow.enabled: true
    color: D.DTK.palette.window
    // 使用 DLayerShellWindow 定位窗口：X轴居中，Y轴锚定底部偏上1/4处
    DS.DLayerShellWindow.anchors: DS.DLayerShellWindow.AnchorBottom
    DS.DLayerShellWindow.layer: DS.DLayerShellWindow.LayerOverlay
    DS.DLayerShellWindow.bottomMargin: (root.screen && root.screen.height > 0) ? Math.max(0, root.screen.height * 0.25 - height * 0.5) : 400

    width: control.implicitWidth
    height: control.implicitHeight
    onClosing: destroy(10)
    Text {
        id: control
        leftPadding: 22
        topPadding: 12
        rightPadding: leftPadding
        bottomPadding: topPadding
        text: root.name
        font: D.DTK.fontManager.t4
        color: D.DTK.palette.brightText
    }
    Shortcut {
        sequence: "Esc"
        onActivated: escPressed()
        onActivatedAmbiguously: escPressed()
    }
}
