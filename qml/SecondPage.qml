// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.2

import org.deepin.dtk 1.0
import org.deepin.dtk.style 1.0 as DStyle

import org.deepin.dcc 1.0

SplitView {
    id: root
    orientation: Qt.Horizontal
    StyledBehindWindowBlur {
        id: leftView
        control: null
        // DccApp.mainWindow()
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        SplitView.preferredWidth: 300

        SearchEdit {
            id: searchEdit
            y: 50
            // anchors.top: title.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            // anchors.horizontalCenter:   parent.horizontalCenter
            anchors.margins: 10

            // Layout.alignment: Qt.AlignHCenter
            // implicitWidth: (parent.width / 2) > 280 ? 280 : (parent.width / 2)
        }
        ListView {
            id: list
            visible: true
            anchors.top: searchEdit.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
            currentIndex: dccObj.children.indexOf(dccObj.currentObject)
            activeFocusOnTab: true
            clip: true
            spacing: 8
            // highlight: Item {
            //     z: 2
            //     FocusBoxBorder {
            //         anchors {
            //             fill: parent
            //             margins: 5
            //         }
            //         radius: 8
            //         color: parent.palette.highlight
            //         // visible: parent.activeFocus
            //     }
            // }
            focus: true
            model: DccModel {
                id: dccModel
                root: dccObj
            }
            delegate: ItemDelegate {
                text: model.display
                width: parent.width
                font: DTK.fontManager.t4
                checked: dccObj.currentObject === model.item
                backgroundVisible: false
                icon {
                    name: model.item.icon
                    width: 48
                    height: 48
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        model.item.trigger();
                        console.log(model.item.name, model.display, model.item.icon)
                    }
                }
            }
        }

    }
    Page {
        SplitView.minimumWidth: 500
        Button {
            id: breakBut
            text: "<"
            anchors.left: parent.left
            // anchors.right: parent.right
            // anchors.top: parent.top
            anchors.verticalCenter: title.verticalCenter
            anchors.margins: 5
            height: 24
            width: 24
            // anchors.bottom: parent.bottom
            // z:2
            onClicked: dccObj.trigger()
            // DccApp.showPage("")
        }
        Label {
            id: title
            // anchors.top: parent.top
            anchors.left: breakBut.right
            anchors.leftMargin: 20
            // anchors.verticalCenter:   parent.verticalCenter
            height: 50
            verticalAlignment: Text.AlignVCenter
            text: DccApp.path
            // linkColor: linkHovered?"red":"green"
            // linkHovered:
            // font.underline : true
            onLinkActivated: function (link) { DccApp.showPage(link) }
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
                leftMargin: 60
                rightMargin: 60
            }
        }
    }
    Component {
        id: menuComponent
        ItemDelegate {
            width: parent.width
            topInset: 5
            bottomInset: 5
            icon {
                name: model.item.icon
                width: 48
                height: 48
            }
            contentFlow: true
            content: RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight : false
                ColumnLayout {
                    Label {
                        text: model.display
                        font: DTK.fontManager.t4
                        elide: Text.ElideRight
                    }
                    Label {
                        visible: text !== ""
                        font: DTK.fontManager.t8
                        text: model.item.description
                    }
                }
                IconLabel {
                    Layout.alignment : Qt.AlignRight
                    Layout.rightMargin : 10
                    icon.name: "arrow_ordinary_right"
                }
            }
            background: DccListViewBackground {
                separatorVisible: false
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    model.item.trigger();
                    console.log(model.item.name, model.display, model.item.icon)
                }
            }
        }
    }
    Component {
        id: editorComponent
        ItemDelegate {
            width: parent.width
            backgroundVisible: false
            checkable: false
            icon.name: model.icon
            text: model.display
            content: Control {
                contentItem: model.item.getSectionItem()
            }
            background: DccListViewBackground {
                separatorVisible: false
            }
        }
    }
    Component {
        id: itemComponent
        Control {
            width: parent.width
            height: contentItem.height
            contentItem: model.item.getSectionItem()
        }
    }
    Component {
        id: rightLayout
        ListView {
            focus: true
            // anchors.topMargin: 10
            // anchors.fill: parent
            // height: contentHeight
            model: DccModel {
                id: dccModel
                root: dccObj
            }
            //禁止listview控件回弹，解决白色空白出现
            boundsBehavior: Flickable.StopAtBounds
            delegate: DelegateChooser {
                role: "pageType"
                DelegateChoice { roleValue: DccObject.Menu; delegate: menuComponent }
                DelegateChoice { roleValue: DccObject.Editor; delegate: editorComponent }
                DelegateChoice { roleValue: DccObject.Item; delegate: itemComponent }
            }
        }
    }
    function updateRightView() {
        var activeObj = DccApp.activeObject
        if (activeObj === dccObj)
            return;

        if (activeObj.page === null) {
            activeObj.page = rightLayout
        }
        rightView.replace(activeObj.getSectionItem())
    }
    Connections {
        target: DccApp
        function onActiveObjectChanged() { updateRightView() }
    }
    Component.onCompleted: updateRightView()
}

