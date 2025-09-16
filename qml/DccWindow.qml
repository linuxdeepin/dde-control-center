// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

import org.deepin.dcc 1.0

D.ApplicationWindow {
    id: root
    property string appProductName: Qt.application.displayName
    property string appLicense: "GPL-3.0-or-later"
    property real currentIndex: 1
    property var sidebarPage: null

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
        blendColor: {
            if (valid) {
                return DS.Style.control.selectColor(control ? control.palette.window : undefined, Qt.rgba(1, 1, 1, 0.8), Qt.rgba(0.06, 0.06, 0.06, 0.8))
            }
            return DS.Style.control.selectColor(undefined, DS.Style.behindWindowBlur.lightNoBlurColor, DS.Style.behindWindowBlur.darkNoBlurColor)
        }
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
        leftContent: D.ActionButton {
            palette.windowText: D.ColorSelector.textColor
            anchors {
                verticalCenter: parent.verticalCenter
                leftMargin: 20
                left: parent.left
            }
            implicitHeight: 30
            implicitWidth: 30
            hoverEnabled: enabled
            activeFocusOnTab: true
            visible: root.currentIndex === 1
            icon {
                name: "sidebar"
                height: 16
                width: 16
            }
            background: Rectangle {
                property D.Palette pressedColor: D.Palette {
                    normal: Qt.rgba(0, 0, 0, 0.2)
                    normalDark: Qt.rgba(1, 1, 1, 0.25)
                }
                property D.Palette hoveredColor: D.Palette {
                    normal: Qt.rgba(0, 0, 0, 0.1)
                    normalDark: Qt.rgba(1, 1, 1, 0.1)
                }
                radius: DS.Style.control.radius
                color: parent.pressed ? D.ColorSelector.pressedColor : (parent.hovered ? D.ColorSelector.hoveredColor : "transparent")
                border {
                    color: parent.palette.highlight
                    width: parent.visualFocus ? DS.Style.control.focusBorderWidth : 0
                }
            }
            onClicked: {
                if (root.sidebarPage) {
                    root.sidebarPage.targetSidebar()
                }
            }
        }
        content: Item {
            anchors {
                fill: parent
                leftMargin: root.currentIndex === 0 ? 0 : -200
            }
            SearchBar {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    verticalCenter: parent.verticalCenter
                }
                visible: root.currentIndex === 0
                model: DccApp.searchModel()
                onClicked: function (model) {
                    DccApp.showPage(model.url)
                }
            }
            D.ActionButton {
                id: breakBut
                palette.windowText: D.ColorSelector.textColor
                implicitHeight: 30
                implicitWidth: 30
                x: ((root.sidebarPage && root.sidebarPage.splitterX > 110) ? root.sidebarPage.splitterX : 110) - 1
                anchors.verticalCenter: parent.verticalCenter
                visible: root.currentIndex === 1
                hoverEnabled: enabled
                activeFocusOnTab: enabled
                enabled: DccApp.activeObject.parentName.length !== 0 && DccApp.activeObject.parentName !== "root"
                onClicked: DccApp.toBack()
                icon {
                    name: "arrow_ordinary_left"
                    height: 12
                    width: 12
                }
                background: Rectangle {
                    property D.Palette pressedColor: D.Palette {
                        normal: Qt.rgba(0, 0, 0, 0.2)
                        normalDark: Qt.rgba(1, 1, 1, 0.25)
                    }
                    property D.Palette hoveredColor: D.Palette {
                        normal: Qt.rgba(0, 0, 0, 0.1)
                        normalDark: Qt.rgba(1, 1, 1, 0.1)
                    }
                    radius: DS.Style.control.radius
                    color: parent.pressed ? D.ColorSelector.pressedColor : (parent.hovered ? D.ColorSelector.hoveredColor : "transparent")
                    border {
                        color: parent.palette.highlight
                        width: parent.visualFocus ? DS.Style.control.focusBorderWidth : 0
                    }
                }
            }
            Crumb {
                width: parent.width - x
                anchors {
                    left: breakBut.right
                    leftMargin: 8
                    verticalCenter: parent.verticalCenter
                }
                visible: root.currentIndex === 1
                model: DccApp.navModel()
                onClicked: function (model) {
                    DccApp.showPage(model.url)
                    centralView.forceActiveFocus()
                }
            }
        }
    }

    Control {
        id: centralView
        hoverEnabled: false
        anchors.fill: parent
    }


    /*  焦点切换调试，暂不删
    onActiveFocusItemChanged: {
        console.log("root onActiveFocusItemChanged:  ", root.activeFocusItem, root.activeFocusControl)
        if (root.activeFocusItem && root.activeFocusControl) {
            console.log("root activeFocusOnTab:  ", root.activeFocusItem.activeFocusOnTab, root.activeFocusControl.activeFocusOnTab)
        }
    }
    D.FocusBoxBorder {
        id: focusIn
        z: 98
        x: {
            if (visible) {
                var p1 = root.activeFocusItem.parent.mapToGlobal(root.activeFocusItem.x, root.activeFocusItem.y)
                console.log("p1:", p1)
                var p2 = mapFromItem(root.activeFocusItem, root.activeFocusItem.x, root.activeFocusItem.y)
                console.log("p2:", p2)
                var p3 = parent.mapFromGlobal(p1.x, p1.y)
                console.log("p3:", p3)
                return p3.x
                // mapFromGlobal(root, root.activeFocusItem.mapToGlobal(
                //                   root.activeFocusItem.x,
                //                   root.activeFocusItem.y)).x
            } else {
                return 0
            }
        }
        y: visible ? parent.mapFromGlobal(root.activeFocusItem.parent.mapToGlobal(root.activeFocusItem.x, root.activeFocusItem.y)).y : 0
        width: visible ? root.activeFocusItem.width : 0
        height: visible ? root.activeFocusItem.height : 0
        // anchors.topMargin:
        visible: root.activeFocusItem

        radius: DS.Style.control.radius
        color: "#80FF0000"
    }
    */
    Component {
        id: rootLayout
        SwipeView {
            id: stackView
            currentIndex: 1
            interactive: false
            activeFocusOnTab: false
            HomePage {
                id: homePage
                visible: false
            }
            SecondPage {
                id: secondPage
                // visible: false
                Component.onCompleted: root.sidebarPage = this
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
                        root.sidebarPage = null
                        stackView.currentIndex = 0
                        root.currentIndex = 0
                        hideTimer.restart()
                    } else if (stackView.currentIndex !== 1 && DccApp.root !== DccApp.activeObject) {
                        homePage.contentVisible = true
                        secondPage.visible = true
                        root.sidebarPage = secondPage
                        stackView.currentIndex = 1
                        root.currentIndex = 1
                        hideTimer.restart()
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        root.width = DccApp.width
        root.height = DccApp.height
        root.x = Screen.virtualX + (Screen.width - root.width) / 2
        root.y = Screen.virtualY + (Screen.height - root.height) / 2
        DccApp.root.page = rootLayout
        centralView.contentItem = DccApp.root.getSectionItem(centralView)
    }
}
