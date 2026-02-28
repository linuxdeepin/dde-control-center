// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

import org.deepin.dcc 1.0

D.ApplicationWindow {
    id: mainWindow
    enum PageIndex {
        LoadIndex,
        HomeIndex,
        SecondIndex
    }

    property string appProductName: Qt.application.displayName
    property string appLicense: "GPL-3.0-or-later"
    property int currentIndex: DccWindow.PageIndex.LoadIndex
    property var sidebarPage: null

    minimumWidth: 520
    minimumHeight: 400
    visible: false
    flags: Qt.Window | Qt.WindowCloseButtonHint | Qt.WindowTitleHint | Qt.WindowMinimizeButtonHint | Qt.WindowMaximizeButtonHint
    modality: Qt.ApplicationModal
    color: "transparent"
    D.DWindow.enabled: true

    MouseArea {
        anchors.fill: parent
        enabled: false
        cursorShape: Qt.ArrowCursor
    }

    D.StyledBehindWindowBlur {
        anchors.fill: parent
        control: mainWindow
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
            visible: mainWindow.currentIndex === DccWindow.PageIndex.SecondIndex
            icon {
                name: "sidebar"
                // Fix: multiply by devicePixelRatio to compensate division in ImageProvider
                // See: dquickimageprovider.cpp boundingSize = requestedSize / devicePixelRatio
                height: 16 * Screen.devicePixelRatio
                width: 16 * Screen.devicePixelRatio
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
                if (mainWindow.sidebarPage) {
                    mainWindow.sidebarPage.targetSidebar()
                }
            }
        }
        content: Item {
            anchors {
                fill: parent
                leftMargin: mainWindow.currentIndex === DccWindow.PageIndex.HomeIndex ? 0 : -200
            }
            SearchBar {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    verticalCenter: parent.verticalCenter
                }
                visible: mainWindow.currentIndex === DccWindow.PageIndex.HomeIndex
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
                x: ((mainWindow.sidebarPage && mainWindow.sidebarPage.splitterX > 110) ? mainWindow.sidebarPage.splitterX : 110) - 1
                anchors.verticalCenter: parent.verticalCenter
                visible: mainWindow.currentIndex === DccWindow.PageIndex.SecondIndex
                hoverEnabled: enabled
                activeFocusOnTab: enabled
                enabled: DccApp.activeObject.parentName.length !== 0 && DccApp.activeObject.parentName !== "root"
                onClicked: DccApp.toBack()
                icon {
                    name: "arrow_ordinary_left"
                    height: 12 * Screen.devicePixelRatio
                    width: 12 * Screen.devicePixelRatio
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
                visible: mainWindow.currentIndex === DccWindow.PageIndex.SecondIndex
                model: DccApp.navModel()
                onClicked: function (model) {
                    DccApp.showPage(model.url)
                    centralView.forceActiveFocus()
                }
            }
        }
    }

    DccLoader {
        id: centralView
        anchors.fill: parent
        dccObj: DccApp.root
    }


    /*  焦点切换调试，暂不删
    onActiveFocusItemChanged: {
        console.log("root onActiveFocusItemChanged:  ", mainWindow.activeFocusItem, mainWindow.activeFocusControl)
        if (mainWindow.activeFocusItem && mainWindow.activeFocusControl) {
            console.log("root activeFocusOnTab:  ", mainWindow.activeFocusItem.activeFocusOnTab, mainWindow.activeFocusControl.activeFocusOnTab)
        }
    }
    D.FocusBoxBorder {
        id: focusIn
        z: 98
        x: {
            if (visible) {
                var p1 = mainWindow.activeFocusItem.parent.mapToGlobal(mainWindow.activeFocusItem.x, mainWindow.activeFocusItem.y)
                console.log("p1:", p1)
                var p2 = mapFromItem(mainWindow.activeFocusItem, mainWindow.activeFocusItem.x, mainWindow.activeFocusItem.y)
                console.log("p2:", p2)
                var p3 = parent.mapFromGlobal(p1.x, p1.y)
                console.log("p3:", p3)
                return p3.x
                // mapFromGlobal(root, mainWindow.activeFocusItem.mapToGlobal(
                //                   mainWindow.activeFocusItem.x,
                //                   mainWindow.activeFocusItem.y)).x
            } else {
                return 0
            }
        }
        y: visible ? parent.mapFromGlobal(mainWindow.activeFocusItem.parent.mapToGlobal(mainWindow.activeFocusItem.x, mainWindow.activeFocusItem.y)).y : 0
        width: visible ? mainWindow.activeFocusItem.width : 0
        height: visible ? mainWindow.activeFocusItem.height : 0
        // anchors.topMargin:
        visible: mainWindow.activeFocusItem

        radius: DS.Style.control.radius
        color: "#80FF0000"
    }
    */
    Component {
        id: rootLayout
        SwipeView {
            id: stackView
            hoverEnabled: false
            currentIndex: DccWindow.PageIndex.LoadIndex
            interactive: false
            activeFocusOnTab: false
            Item {
                id: loadPage
                D.DciIcon {
                    anchors.centerIn: parent
                    name: "control-loading"
                    mode: D.DTK.NormalState
                    theme: D.DTK.themeType
                }
            }
            HomePage {
                id: homePage
                visible: false
            }
            SecondPage {
                id: secondPage
                // visible: false
                Component.onCompleted: mainWindow.sidebarPage = this
            }
            Connections {
                target: DccApp
                function onActiveObjectChanged(activeObject) {
                    if (stackView.currentIndex !== DccWindow.PageIndex.LoadIndex && null === DccApp.activeObject) {
                        homePage.contentVisible = true
                        secondPage.visible = true
                        mainWindow.sidebarPage = null
                        stackView.currentIndex = DccWindow.PageIndex.LoadIndex
                        mainWindow.currentIndex = DccWindow.PageIndex.LoadIndex
                    } else if (stackView.currentIndex !== DccWindow.PageIndex.HomeIndex && DccApp.root === DccApp.activeObject) {
                        homePage.contentVisible = true
                        secondPage.visible = true
                        mainWindow.sidebarPage = null
                        stackView.currentIndex = DccWindow.PageIndex.HomeIndex
                        mainWindow.currentIndex = DccWindow.PageIndex.HomeIndex
                    } else if (stackView.currentIndex !== DccWindow.PageIndex.SecondIndex && DccApp.root !== DccApp.activeObject) {
                        homePage.contentVisible = true
                        secondPage.visible = true
                        mainWindow.sidebarPage = secondPage
                        stackView.currentIndex = DccWindow.PageIndex.SecondIndex
                        mainWindow.currentIndex = DccWindow.PageIndex.SecondIndex
                    }
                }
            }
            Component.onCompleted: {
                if (contentItem) {
                    contentItem.highlightMoveDuration = 0
                }
            }
        }
    }

    Component.onCompleted: {
        mainWindow.width = DccApp.width
        mainWindow.height = DccApp.height
        mainWindow.x = Screen.virtualX + (Screen.width - mainWindow.width) / 2
        mainWindow.y = Screen.virtualY + (Screen.height - mainWindow.height) / 2
        DccApp.root.page = rootLayout
    }
}
