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
    property string appLicense: "LGPL-3.0-or-later"

    minimumWidth: 500
    minimumHeight: 200
    visible: false
    flags: Qt.Window | Qt.WindowCloseButtonHint | Qt.WindowTitleHint | Qt.WindowMinimizeButtonHint | Qt.WindowMaximizeButtonHint
    modality: Qt.ApplicationModal
    color: "transparent"
    D.DWindow.enabled: true

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
                aboutDialog: titleBar.aboutDialog
            }
            D.QuitAction {}
        }
        aboutDialog: D.AboutDialog {
            id: aboutDialog
            D.DWindow.enabled: true
            productIcon: "preferences-system"
            modality: Qt.NonModal
            productName: appProductName
            companyLogo: "file://" + DTK.deepinDistributionOrgLogo
            websiteName: DTK.deepinWebsiteName
            websiteLink: DTK.deepinWebsiteLink
            description: qsTr("Control Center provides the options for system settings.")
            license: appLicense === "" ? "" : qsTr("%1 is released under %2").arg(appProductName).arg(appLicense)
            onClosing: destroy(10)
        }
        embedMode: false
        autoHideOnFullscreen: true
        focus: true
        Keys.onSpacePressed: Window.window.visibility = Window.FullScreen
    }

    Control {
        id: centralView
        focusPolicy: Qt.TabFocus
        hoverEnabled: false
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
                visible: false
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
