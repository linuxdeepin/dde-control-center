// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0
import org.deepin.dtk.style 1.0 as DStyle

import org.deepin.dcc 1.0

SplitView {
    id: root
    orientation: Qt.Horizontal
    handle: Rectangle {
        implicitWidth: 2
        color: palette.light // "#B9DEFB"
    }

    StyledBehindWindowBlur {
        id: leftView
        control: null // DccApp.mainWindow()
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        SplitView.preferredWidth: 300

        SearchEdit {
            id: searchEdit
            y: 50
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
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
            clip: true
            spacing: 8
            focus: true
            model: DccModel {
                id: dccModel
                root: dccObj
            }
            delegate: ItemDelegate {
                text: model.display
                width: parent ? parent.width : 300
                checked: dccObj.currentObject === model.item
                backgroundVisible: false
                icon {
                    name: model.item.icon
                    width: 48
                    height: 48
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
                onClicked: {
                    model.item.trigger()
                    console.log(model.item.name, model.display, model.item.icon)
                }
            }
        }
    }

    Control {
        SplitView.minimumWidth: 500
        Item {
            id: header
            implicitHeight: 50
            ToolButton {
                id: breakBut
                icon.name: "arrow_ordinary_left"
                anchors {
                    left: parent.right
                    verticalCenter: parent.verticalCenter
                    margins: 10
                }
                implicitHeight: 16
                implicitWidth: 16
                onClicked: dccObj.trigger()
            }

            Crumb {
                implicitHeight: parent.implicitHeight
                implicitWidth: 160
                anchors {
                    left: breakBut.right
                    leftMargin: 40
                    // right: parent.right
                    rightMargin: 200
                }
                model: DccApp.navModel()
                onClicked: function (model) {
                    DccApp.showPage(model.url)
                }
            }
        }
        StackView {
            anchors.top: header.bottom
            anchors.topMargin: header.height
            id: rightView
            clip: true
            anchors.fill: parent
        }

        Rectangle {
            id: background
            z: -1
            anchors.fill: parent
            color: palette.window
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
        rightView.replace(activeObj.getSectionItem())
    }
    Connections {
        target: DccApp
        function onActiveObjectChanged() {
            updateRightView()
        }
    }
    Component.onCompleted: updateRightView()
}
