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
    flags: Qt.Window | Qt.WindowCloseButtonHint | Qt.WindowTitleHint | Qt.WindowMinimizeButtonHint | Qt.WindowMaximizeButtonHint
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
            HomePage {
                id: homePage
            }
            SecondPage {
                id: secondPage
            }
            Timer {
                id: hideTimer
                interval: 500
                repeat: false
                onTriggered: {
                    homePage.visible = stackView.currentIndex === 0
                    secondPage.visible = stackView.currentIndex === 1
                }
            }
            Connections {
                target: DccApp
                function onActiveObjectChanged(activeObject) {
                    if (stackView.currentIndex !== 0 && DccApp.root === DccApp.activeObject) {
                        homePage.visible = true
                        secondPage.visible = true
                        stackView.currentIndex = 0
                        hideTimer.restart()
                    } else if (stackView.activePage !== 1 && DccApp.root !== DccApp.activeObject) {
                        homePage.visible = true
                        secondPage.visible = true
                        stackView.currentIndex = 1
                        hideTimer.restart()
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        root.width = 700 //DccApp.width
        root.height = 472 //DccApp.height
        root.x = (Screen.width - root.width) / 2
        root.y = (Screen.height - root.height) / 2
        DccApp.root.page = rootLayout
        centralView.contentItem = DccApp.root.getSectionItem()
    }
}
