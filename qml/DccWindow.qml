// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3

import org.deepin.dtk 1.0

import Dcc 1.0

Window {
    id: root
    visible: false
    // flags: Qt.FramelessWindowHint | Qt.Window
    // flags: Qt.FramelessWindowHint| Qt.WindowMinMaxButtonsHint | Qt.WindowCloseButtonHint | Qt.WindowTitleHint | Qt.Window
    flags: Qt.Window| Qt.WindowCloseButtonHint | Qt.WindowTitleHint | Qt.WindowMinimizeButtonHint | Qt.WindowMaximizeButtonHint
    property int bw: 3
    modality: Qt.ApplicationModal
    // color: "#f8f8f8"
    color: "transparent"
    DWindow.enabled: true

    DciIcon {
        z: 10
        height: 50
        width: 50
        anchors.left: parent.left
        anchors.top: parent.top
        name: "preferences-system"
        sourceSize: Qt.size(32, 32)
    }
    TitleBar {
        id: titleBar
        // z:10
        // anchors.right: parent.right
        // anchors.top: parent.top
    }
    Item {
        anchors.fill: titleBar
        DragHandler {
            grabPermissions: TapHandler.CanTakeOverFromAnything
            onActiveChanged: if (active) { root.startSystemMove(); }
        }
    }

    Control {
        id: centralView
        // padding: 100
        focusPolicy:Qt.TabFocus
        anchors.fill: parent
    }
    Component {
        id: rootLayout
        SwipeView{
            // StackView {
            id: stackView
            currentIndex: 0
            interactive: false
            // highlightMoveDuration : 0
            HomePage{ }
            SecondPage{ }
            // property int activePage: -1
            // initialItem:  DccApp.root.currentObject == null? "HomePage.qml" :"Page2.qml"
            // anchors.fill: parent
            Connections {
                target: DccApp
                onActiveObjectChanged: {
                    console.log(DccApp.root)
                    console.log(DccApp.activeObject)
                    if (stackView.currentIndex !== 0 && DccApp.root === DccApp.activeObject) {
                        // stackView.push("HomePage.qml")
                        stackView.currentIndex = 0
                    } else if (stackView.activePage !== 1 && DccApp.root !== DccApp.activeObject) {
                        stackView.currentIndex = 1
                        // stackView.push("Page2.qml")
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
        console.warn("==========root.flags=======")
        console.warn(root.flags)
        console.log(root.flags)
    }
}
