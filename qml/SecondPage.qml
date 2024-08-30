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
        SplitView.preferredWidth: 180

        SearchBar {
            id: searchEdit
            anchors {
                left: parent.left
                right: parent.right
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

    Rectangle {
        SplitView.minimumWidth: 500
        color: palette.window
        RowLayout {
            id: header
            implicitHeight: 50
            anchors {
                left: parent.left
                right: parent.right
            }
            ToolButton {
                id: breakBut
                icon.name: "arrow_ordinary_left"
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                Layout.margins: 10
                implicitHeight: 16
                implicitWidth: 16
                onClicked: dccObj.trigger()
            }

            Crumb {
                implicitHeight: parent.implicitHeight
                implicitWidth: 160
                Layout.fillWidth: true
                Layout.leftMargin: 40
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
