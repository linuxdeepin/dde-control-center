// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

import org.deepin.dcc 1.0

Item {
    id: root
    property real oldX: 180 // 用于调整宽度时，自动调整侧边栏宽度
    property real oldSplitterX: 180 // 记录上次位置，用于恢复
    property alias splitterX: splitter.x

    function targetSidebar() {
        if (splitter.x < 110) {
            var newX = root.oldSplitterX
            if (root.width - newX > 520) {
                splitter.x = newX
            } else if (root.width - 180 > 520) {
                splitter.x = 180
            } else if (root.width - 110 > 520) {
                splitter.x = 110
            } else {
                let dx = 630 - root.width
                DccApp.mainWindow().x -= dx
                DccApp.mainWindow().width = 630
                splitter.x = 110
            }
        } else {
            root.oldSplitterX = splitter.x
            splitter.x = 0
            root.oldX = 0
        }
    }

    activeFocusOnTab: false

    Item {
        id: leftView
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: splitter.left
        }
        visible: width > 20
        SearchBar {
            id: searchEdit
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: 10
                topMargin: 50
            }

            model: DccApp.searchModel()
            onClicked: function (model) {
                DccApp.showPage(model.url)
            }
        }
        ListView {
            id: list
            visible: true
            anchors.top: searchEdit.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 3
            leftMargin : 10
            rightMargin: 10
            topMargin: 6
            bottomMargin: 10
            currentIndex: dccObj ? dccObj.children.indexOf(dccObj.currentObject) : -1
            activeFocusOnTab: true
            clip: true
            ScrollBar.vertical: ScrollBar {
                width: 10
            }
            model: DccModel {
                id: dccModel
                root: dccObj
            }

            delegate: D.ItemDelegate {
                implicitHeight: 40
                width: parent ? parent.width : 300
                checked: dccObj.currentObject === model.item
                font: D.DTK.fontManager.t6
                icon {
                    name: model.item.icon
                    source: model.item.iconSource
                    width: 24
                    height: 24
                }
                contentFlow: true
                content: RowLayout {
                    DccLabel {
                        Layout.fillWidth: true
                        text: model.display
                    }
                    Rectangle {
                        Layout.alignment: Qt.AlignRight
                        Layout.rightMargin: 10
                        visible: model.item.badge !== 0
                        height: 16
                        width: 16
                        radius: 8
                        color: "red"
                    }
                }
                hoverEnabled: true
                background: DccItemBackground {
                    separatorVisible: false
                    bgMargins: 0
                    backgroundType: DccObject.Hover | DccObject.Clickable
                    backgroundColor: D.Palette {
                        normal: Qt.rgba(1, 1, 1, 1)
                        normalDark: Qt.rgba(1, 1, 1, 0.05)
                        hovered: Qt.rgba(0, 0, 0, 0.1)
                        hoveredDark: Qt.rgba(1, 1, 1, 0.1)
                        pressed: Qt.rgba(0, 0, 0, 0.2)
                        pressedDark: Qt.rgba(1, 1, 1, 0.25)
                    }
                }
                onClicked: {
                    DccApp.showPage(model.item)
                    console.log(model.item.name, model.display, model.item.icon)
                }
            }
        }
    }
    Rectangle {
        property D.Palette bgColor: D.Palette {
            normal: Qt.rgba(0.97, 0.97, 0.97, 0.95)
            normalDark: Qt.rgba(0.09, 0.09, 0.09, 0.85)
        }
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: splitter.right
            right: parent.right
        }
        color: D.ColorSelector.bgColor
        StackView {
            id: rightView
            clip: true
            hoverEnabled: true
            anchors {
                fill: parent
                topMargin: 50
            }
        }
    }
    Rectangle {
        id: splitter
        property D.Palette bgColor: D.Palette {
            normal: Qt.rgba(0, 0, 0, 0.05)
            normalDark: Qt.rgba(0, 0, 0, 0.5)
        }
        implicitWidth: 1
        x: 180
        height: root.height
        color: D.ColorSelector.bgColor
    }
    MouseArea {
        x: splitter.x - 2
        width: 5
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        cursorShape: Qt.SizeHorCursor
        onPositionChanged: function (mouse) {
            var newX = mouse.x + splitter.x
            if (newX >= 0 && newX < root.width - splitter.width) {
                if (root.width - newX < 520) {
                    return
                }
                if (newX < 110) {
                    newX = 0
                }

                splitter.x = newX
                root.oldX = newX
            }
        }
    }

    onWidthChanged: {
        var newX = width - 510
        if (width - splitter.x < 510) {
            if (newX < 0) {
                return
            }
            if (newX < 110) {
                newX = 0
            }
            splitter.x = newX
        } else if (splitter.x < oldX) {
            newX = width - 510
            if (newX < 0) {
                return
            }
            if (newX < 110) {
                newX = 0
            }
            if (newX > oldX) {
                newX = oldX
            }
            splitter.x = newX
        }
    }
    Component {
        id: rightLayout
        DccRightView {}
    }

    function updateRightView() {
        var activeObj = DccApp.activeObject
        if (activeObj === dccObj) {
            return
        }
        if (activeObj.page === null) {
            activeObj.page = rightLayout
        }
        rightView.replace(activeObj.getSectionItem(rightView), DccApp.animationMode === DccApp.AnimationPush
                          ? StackView.PushTransition
                          : StackView.PopTransition)
    }
    Connections {
        target: DccApp
        function onActiveObjectChanged() {
            updateRightView()
        }
    }
    Component.onCompleted: updateRightView()
}
