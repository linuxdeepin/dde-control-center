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
    property var activeObject: null
    orientation: Qt.Horizontal
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
                font: DTK.fontManager.t4
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
    Page {
        SplitView.minimumWidth: 500
        ToolButton {
            id: breakBut
            icon.name: "arrow_ordinary_left"
            anchors.left: parent.left
            anchors.verticalCenter: title.verticalCenter
            anchors.margins: 10
            height: 16
            width: 16
            onClicked: dccObj.trigger()
        }
        Label {
            id: title
            anchors.left: breakBut.right
            anchors.leftMargin: 40
            height: 50
            verticalAlignment: Text.AlignVCenter
            text: DccApp.path
            onLinkActivated: function (link) {
                DccApp.showPage(link)
            }
            onLinkHovered: function (link) {
                console.log(link)
            }
        }
        StackView {
            id: rightView
            clip: true
            anchors {
                left: parent.left
                right: parent.right
                top: title.bottom
                bottom: parent.bottom
            }
        }
    }
    Component {
        id: rightLayout
        DccRightView {}
    }
    function updateActiveObj(obj: var) {
        if (activeObject !== null) {
            console.log("deactive", activeObject)
            activeObject.deactive()
        }
        activeObject = obj
    }

    function updateRightView() {
        var activeObj = DccApp.activeObject
        if (activeObj === dccObj) {
            updateActiveObj(null)
            return
        }

        if (activeObj.page === null) {
            activeObj.page = rightLayout
        }
        updateActiveObj(activeObj)
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
