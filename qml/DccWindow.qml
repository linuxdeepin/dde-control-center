// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3

import org.deepin.dtk 1.0 as D

import org.deepin.dcc 1.0

D.ApplicationWindow {
    id: root
    visible: false
    flags: Qt.Window | Qt.WindowCloseButtonHint | Qt.WindowTitleHint
           | Qt.WindowMinimizeButtonHint | Qt.WindowMaximizeButtonHint
    modality: Qt.ApplicationModal
    color: "transparent"
    DWindow.enabled: true

    D.TitleBar {
        id: titleBar
        icon.name: "preferences-system"
        focusPolicy: Qt.TabFocus
        activeFocusOnTab: true
    }
    Item {
        anchors.fill: titleBar
        DragHandler {
            grabPermissions: TapHandler.CanTakeOverFromAnything
            onActiveChanged: function () {
                if (active) {
                    root.startSystemMove()
                }
            }
        }
    }

    Control {
        id: centralView
        focusPolicy: Qt.TabFocus
        anchors.fill: parent
    }
    Component {
        id: rootLayout
        SwipeView {
            id: stackView
            currentIndex: 0
            interactive: false
            HomePage {}
            SecondPage {}
            Connections {
                target: DccApp
                function onActiveObjectChanged(activeObject) {
                    if (stackView.currentIndex !== 0
                            && DccApp.root === DccApp.activeObject) {
                        stackView.currentIndex = 0
                    } else if (stackView.activePage !== 1
                               && DccApp.root !== DccApp.activeObject) {
                        stackView.currentIndex = 1
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        root.width = DccApp.width
        root.height = DccApp.height
        root.x = (Screen.width - root.width) / 2
        root.y = (Screen.height - root.height) / 2
        DccApp.root.page = rootLayout
        centralView.contentItem = DccApp.root.getSectionItem()
    }
}
