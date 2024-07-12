// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1

import org.deepin.dtk 1.0 as D

import org.deepin.dcc 1.0
import DefApp 1.0

DccObject {
    id: root
    property DccObject parentObj: null
    property CategoryModel categoryModel: null
    property bool canDelete: false
    DccObject {
        name: "title"
        parentName: parentObj.name
        displayName: qsTr("选择打开“ ") + parentObj.displayName + qsTr("”的默认程序 ")
        weight: 10
        pageType: DccObject.Editor
        page: RowLayout {
            FileDialog {
                id: fileDialog
                title: qsTr("Open Desktop file")
                folder: "/usr/share/applications"
                visible: false
                nameFilters: [qsTr("Apps (*.desktop)"), qsTr("All files (*)")]
                onAccepted: {
                    categoryModel.addApp(fileDialog.currentFile)
                }
            }

            Button {
                Layout.alignment : Qt.AlignRight
                Layout.maximumWidth: 30
                Layout.maximumHeight: 30
                Layout.margins: 0
                enabled: canDelete
                text: "-"
                onClicked: {
                    console.log(parentObj.name, "-")
                }
            }
            Button {
                Layout.alignment : Qt.AlignRight
                Layout.maximumWidth: 30
                Layout.maximumHeight: 30
                Layout.margins: 0
                text: "+"
                onClicked: {
                    fileDialog.open()
                }
                onVisibleChanged: {
                    fileDialog.close()
                }
            }
        }
    }
    DccObject {
        name: "appList"
        parentName: parentObj.name
        weight: 20
        pageType: DccObject.Item
        page: ListView {
            id: view
            clip: true
            activeFocusOnTab: true
            currentIndex: -1
            focus: true
            height: contentHeight
            spacing: 0
            Keys.enabled: true
            Keys.onPressed: function(event) {
                switch (event.key) {
                    case Qt.Key_Plus:
                        console.log(event.key, currentIndex)
                        break
                    case Qt.Key_Space:
                    case Qt.Key_Enter:
                    case Qt.Key_Return:
                        console.log(event.key, currentIndex)
                        break
                    default:
                        return
                }
                event.accepted = true
            }
            onCurrentIndexChanged: function () {
                if (currentIndex > 0) {
                    canDelete = currentItem.canDelete
                }
            }
            model: categoryModel
            // /*
            delegate: ItemDelegate {
                property string name: model.id
                property bool canDelete: model.canDelete
                text: model.display
                icon.name: model.icon
                checked: true
                cascadeSelected: index !== currentIndex
                anchors.left: parent.left
                anchors.right: parent.right
                // checkable: false
                indicatorVisible: model.isDefault
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (!model.isDefault) {
                            categoryModel.setDefaultApp(model.id)
                        }
                    }
                }
                background: DccListViewBackground { }
            }
        }
    }
}
