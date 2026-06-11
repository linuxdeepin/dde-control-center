// SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15

import org.deepin.ds 1.0 as DS
import org.deepin.dcc 1.0

Loader {
    id: root
    property var screen: null
    signal closed
    active: screen !== null
    Timer {
        interval: 1000
        running: root.active
        onTriggered: root.closed()
    }

    component IndicatorBorder: Window {
        screen: root.screen
        color: "#2ca7f8"

        flags: Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint
        DS.DLayerShellWindow.layer: DS.DLayerShellWindow.LayerOverlay
        DS.DLayerShellWindow.exclusionZone: -1
        Component.onCompleted: show()
        height: 10
        width: 10
    }

    sourceComponent: IndicatorBorder {
        DS.DLayerShellWindow.anchors: DS.DLayerShellWindow.AnchorTop
                                      | DS.DLayerShellWindow.AnchorLeft
                                      | DS.DLayerShellWindow.AnchorRight
        IndicatorBorder {
            DS.DLayerShellWindow.anchors: DS.DLayerShellWindow.AnchorBottom
                                          | DS.DLayerShellWindow.AnchorLeft
                                          | DS.DLayerShellWindow.AnchorRight
        }
        IndicatorBorder {
            DS.DLayerShellWindow.anchors: DS.DLayerShellWindow.AnchorLeft
                                          | DS.DLayerShellWindow.AnchorTop
                                          | DS.DLayerShellWindow.AnchorBottom
        }
        IndicatorBorder {
            DS.DLayerShellWindow.anchors: DS.DLayerShellWindow.AnchorRight
                                          | DS.DLayerShellWindow.AnchorTop
                                          | DS.DLayerShellWindow.AnchorBottom
        }
    }
}
