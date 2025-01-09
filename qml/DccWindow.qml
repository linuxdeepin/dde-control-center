// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3

import org.deepin.dtk 1.0 as D

import org.deepin.dcc 1.0

D.ApplicationWindow {
    id: root
    property string appProductName: Qt.application.displayName
    property string appLicense: "GPL-3.0-or-later"

    minimumWidth: 520
    minimumHeight: 400
    visible: false
    flags: Qt.Window | Qt.WindowCloseButtonHint | Qt.WindowTitleHint | Qt.WindowMinimizeButtonHint | Qt.WindowMaximizeButtonHint
    modality: Qt.ApplicationModal
    color: "transparent"
    D.DWindow.enabled: true

    D.StyledBehindWindowBlur {
        anchors.fill: parent
        control: root
    }
    Shortcut {
        context: Qt.ApplicationShortcut
        sequences: [StandardKey.HelpContents, "F1"]
        onActivated: DccApp.showHelp()
        onActivatedAmbiguously: DccApp.showHelp()
    }
    D.TitleBar {
        id: titleBar
        icon.name: "preferences-system"
        focusPolicy: Qt.TabFocus
        activeFocusOnTab: true
        implicitHeight: 40
        menu: Menu {
            D.ThemeMenu {}
            D.MenuSeparator {}
            D.HelpAction {
                onTriggered: DccApp.showHelp()
            }
            D.AboutAction {
                aboutDialog: D.AboutDialog {
                    D.DWindow.enabled: true
                    productIcon: "preferences-system"
                    modality: Qt.NonModal
                    productName: appProductName
                    websiteName: DTK.deepinWebsiteName
                    websiteLink: DTK.deepinWebsiteLink
                    description: qsTr("Control Center provides the options for system settings.")
                    onClosing: destroy(10)
                }
            }
            D.QuitAction {}
        }
        embedMode: false
        autoHideOnFullscreen: true
        focus: true
    }

    Control {
        id: centralView
        focusPolicy: Qt.TabFocus
        hoverEnabled: false
        anchors.fill: parent
    }

    // FIXME：any better way ?
    MouseArea {
        z: 99
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton

        onPressed: function (mouse) {
            mouse.accepted = false

            if (!root.activeFocusItem)
                return

            let pt = mapToItem(root.activeFocusItem, mouse.x, mouse.y)
            // clear focus if click out of activeFocusItem
            if (!root.activeFocusItem.contains(pt)) {
                root.activeFocusItem.focus = false

                // if focus = false not work (e.g. password spinbox edit, focus scope, flags with ItemIsFocusScope)
                // parent focus set false
                if (root.activeFocusItem.parent)
                    root.activeFocusItem.parent.focus = false
            }
        }
    }

    Component {
        id: rootLayout
        SwipeView {
            id: stackView
            currentIndex: 1
            interactive: false
            HomePage {
                id: homePage
                visible: false
            }
            SecondPage {
                id: secondPage
                // visible: false
            }
            Timer {
                id: hideTimer
                interval: 500
                repeat: false
                onTriggered: {
                    homePage.contentVisible = stackView.currentIndex === 0
                    secondPage.visible = stackView.currentIndex === 1
                }
            }
            Connections {
                target: DccApp
                function onActiveObjectChanged(activeObject) {
                    if (stackView.currentIndex !== 0 && DccApp.root === DccApp.activeObject) {
                        homePage.contentVisible = true
                        secondPage.visible = true
                        stackView.currentIndex = 0
                        hideTimer.restart()
                    } else if (stackView.currentIndex !== 1 && DccApp.root !== DccApp.activeObject) {
                        homePage.contentVisible = true
                        secondPage.visible = true
                        stackView.currentIndex = 1
                        hideTimer.restart()
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
        centralView.contentItem = DccApp.root.getSectionItem(centralView)
    }
}
