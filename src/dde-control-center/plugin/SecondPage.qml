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

    function updateSidebarWidth(sidebarWidth) {
        if (!rightView.empty) {
            DccApp.sidebarWidth = sidebarWidth
        }
    }
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
        updateSidebarWidth(splitter.x)
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
            leftMargin: 10
            rightMargin: 10
            topMargin: 6
            bottomMargin: 10
            clip: true
            focus: true
            activeFocusOnTab: true
            currentIndex: dccObj ? dccObj.children.indexOf(dccObj.currentObject) : -1

            MouseArea {
                anchors.fill: parent
                z: 999
                acceptedButtons: Qt.LeftButton
                propagateComposedEvents: true
                onPressed: function(mouse) {
                    list.forceActiveFocus()
                    mouse.accepted = false
                }
            }

            ScrollBar.vertical: ScrollBar { width: 10 }

            Keys.enabled: true
            Keys.onPressed: function (event) {
                switch (event.key) {
                case Qt.Key_Up:
                    if (count <= 0 || !dccObj)
                        break
                    var upIdx = currentIndex
                    if (upIdx < 0)
                        upIdx = 0
                    else if (upIdx > 0)
                        upIdx--
                    var upObj = dccModel.getObject(upIdx)
                    if (upObj) {
                        dccObj.currentObject = upObj
                        DccApp.showPage(upObj)
                    }
                    break
                case Qt.Key_Down:
                    if (count <= 0 || !dccObj)
                        break
                    var downIdx = currentIndex
                    if (downIdx < 0)
                        downIdx = 0
                    else if (downIdx < count - 1)
                        downIdx++
                    var downObj = dccModel.getObject(downIdx)
                    if (downObj) {
                        dccObj.currentObject = downObj
                        DccApp.showPage(downObj)
                    }
                    break
                case Qt.Key_Enter:
                case Qt.Key_Return:
                    var obj = dccModel.getObject(currentIndex)
                    if (obj)
                        DccApp.showPage(obj)
                    break
                default:
                    return
                }
                event.accepted = true
            }

            model: DccModel {
                id: dccModel
                root: dccObj
            }

            delegate: D.ItemDelegate {
                implicitHeight: 40
                width: parent ? parent.width : 300
                font: D.DTK.fontManager.t6
                activeFocusOnTab: false
                focusPolicy: Qt.NoFocus

                checked: dccObj.currentObject === model.item
                cascadeSelected: false

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
                    D.DciIcon {
                        visible: model.item.badge !== 0
                        name: "reddot"
                        sourceSize: Qt.size(16, 16)
                    }
                }
                hoverEnabled: true

                background: DccItemBackground {
                    focusBorderVisible: false
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
                    list.forceActiveFocus()
                    DccApp.showPage(model.item)
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
            updateSidebarWidth(splitter.x)
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
        updateSidebarWidth(splitter.x)
    }
    Component {
        id: rightLayout
        DccRightView {}
    }
    Component {
        id: mainView
        DccLoader {}
    }
    function updateRightView() {
        var activeObj = DccApp.activeObject
        if (activeObj === dccObj) {
            return
        }
        if (activeObj.page === null) {
            activeObj.page = rightLayout
        }
        rightView.replace(mainView, {
                              "dccObj": activeObj
                          }, DccApp.animationMode === DccApp.AnimationPush ? StackView.PushTransition : StackView.PopTransition)
        var rootFirstItem = DccApp.root.children.length > 0 ? DccApp.root.children[0] : null
        if (activeObj !== rootFirstItem) {
            list.forceActiveFocus()
        }
    }
    Connections {
        target: DccApp
        function onActiveObjectChanged() {
            updateRightView()
        }
    }
    Component.onCompleted: {
        oldX = DccApp.sidebarWidth
        updateRightView()
    }
}
