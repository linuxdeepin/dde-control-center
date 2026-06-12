// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15

import org.deepin.ds 1.0 as DS
import org.deepin.dcc 1.0

Window {
    id: root
    flags: Qt.CoverWindow | Qt.WindowStaysOnTopHint | Qt.SplashScreen | Qt.FramelessWindowHint | Qt.X11BypassWindowManagerHint
    color: "#2ca7f8"
    // 顶部边框：锚定顶部，左右拉伸
    DS.DLayerShellWindow.anchors: DS.DLayerShellWindow.AnchorTop | DS.DLayerShellWindow.AnchorLeft | DS.DLayerShellWindow.AnchorRight
    DS.DLayerShellWindow.layer: DS.DLayerShellWindow.LayerOverlay
    DS.DLayerShellWindow.exclusionZone: 0
    width: 10
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
        // 底部边框：锚定底部，左右拉伸
        DS.DLayerShellWindow.anchors: DS.DLayerShellWindow.AnchorBottom | DS.DLayerShellWindow.AnchorLeft | DS.DLayerShellWindow.AnchorRight
        DS.DLayerShellWindow.layer: DS.DLayerShellWindow.LayerOverlay
        DS.DLayerShellWindow.exclusionZone: 0
        width: 10
        height: 10
    }
    Window {
        flags: Qt.CoverWindow | Qt.WindowStaysOnTopHint | Qt.SplashScreen | Qt.FramelessWindowHint | Qt.X11BypassWindowManagerHint
        visible: root.visible
        color: root.color
        screen: root.screen
        // 左侧边框：锚定左侧，上下拉伸
        DS.DLayerShellWindow.anchors: DS.DLayerShellWindow.AnchorLeft | DS.DLayerShellWindow.AnchorTop | DS.DLayerShellWindow.AnchorBottom
        DS.DLayerShellWindow.layer: DS.DLayerShellWindow.LayerOverlay
        DS.DLayerShellWindow.exclusionZone: 0
        width: 10
        height: 10
    }
    Window {
        flags: Qt.CoverWindow | Qt.WindowStaysOnTopHint | Qt.SplashScreen | Qt.FramelessWindowHint | Qt.X11BypassWindowManagerHint
        visible: root.visible
        color: root.color
        screen: root.screen
        // 右侧边框：锚定右侧，上下拉伸
        DS.DLayerShellWindow.anchors: DS.DLayerShellWindow.AnchorRight | DS.DLayerShellWindow.AnchorTop | DS.DLayerShellWindow.AnchorBottom
        DS.DLayerShellWindow.layer: DS.DLayerShellWindow.LayerOverlay
        DS.DLayerShellWindow.exclusionZone: 0
        width: 10
        height: 10
    }
}
