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
    property real oldX: 180
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
            anchors.margins: 10
            currentIndex: dccObj ? dccObj.children.indexOf(dccObj.currentObject) : -1
            activeFocusOnTab: true
            model: DccModel {
                id: dccModel
                root: dccObj
            }
            delegate: ItemDelegate {
                implicitHeight: 40
                text: model.display
                width: parent ? parent.width : 300
                checked: dccObj.currentObject === model.item
                font: D.DTK.fontManager.t6
                icon {
                    name: model.item.icon
                    source: model.item.iconSource
                    width: 24
                    height: 24
                }
                content: Rectangle {
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    visible: model.item.badge !== 0
                    height: 16
                    width: 16
                    radius: 8
                    color: "red"
                }
                hoverEnabled: true
                background: DccItemBackground {
                    separatorVisible: false
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
        RowLayout {
            id: header
            implicitHeight: 50
            anchors {
                left: parent.left
                right: parent.right
            }
            Item {
                implicitWidth: splitter.x < 110 ? 110 - splitter.x : 0
            }
            D.ActionButton {
                id: breakBut
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                Layout.margins: 10
                implicitHeight: 30
                implicitWidth: 30
                hoverEnabled: enabled
                activeFocusOnTab: true
                enabled: DccApp.activeObject.parentName.length !== 0 && DccApp.activeObject.parentName !== "root"
                onClicked: DccApp.toBack()
                icon {
                    name: "arrow_ordinary_left"
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
            }

            Crumb {
                implicitHeight: parent.implicitHeight
                implicitWidth: 160
                Layout.fillWidth: true
                Layout.leftMargin: 0
                Layout.rightMargin: 200
                model: DccApp.navModel()
                onClicked: function (model) {
                    DccApp.showPage(model.url)
                }
            }
        }
        StackView {
            id: rightView
            clip: true
            anchors {
                top: header.bottom
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
        }
    }
    RowLayout {
        height: 50
        implicitWidth: 100
        D.ActionButton {
            property real oldSplitterX: 180
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            Layout.leftMargin: 60
            implicitHeight: 30
            implicitWidth: 30
            hoverEnabled: enabled
            activeFocusOnTab: true
            KeyNavigation.tab: splitter.x < 110 ? (breakBut.enabled ? breakBut : breakBut.nextItemInFocusChain()) : searchEdit.edit
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
                if (splitter.x < 110) {
                    var newX = oldSplitterX
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
                    oldSplitterX = splitter.x
                    splitter.x = 0
                    root.oldX = 0
                }
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
        rightView.replace(activeObj.getSectionItem(rightView))
    }
    Connections {
        target: DccApp
        function onActiveObjectChanged() {
            updateRightView()
        }
    }
    Component.onCompleted: updateRightView()
}
