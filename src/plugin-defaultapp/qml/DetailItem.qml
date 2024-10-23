// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import Qt.labs.qmlmodels 1.2

import org.deepin.dtk 1.0 as D

import org.deepin.dcc 1.0
import org.deepin.dcc.defApp 1.0

DccObject {
    id: root
    property DccObject parentObj: null
    property CategoryModel categoryModel: null
    property bool canDelete: false
    DccObject {
        name: "title"
        parentName: parentObj.name
        displayName: qsTr("Please choose the default program to open '%1'").arg(parentObj.displayName)
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
                Layout.alignment: Qt.AlignRight
                Layout.maximumWidth: 30
                Layout.maximumHeight: 30
                Layout.margins: 0
                enabled: canDelete
                icon.name: "action_reduce"
                onClicked: {
                    console.log(parentObj.name, "-")
                }
                Component.onCompleted: {
                    enabled = false
                    update()
                }
            }
            Button {
                Layout.alignment: Qt.AlignRight
                Layout.maximumWidth: 30
                Layout.maximumHeight: 30
                Layout.margins: 0
                icon.name: "action_add"
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
        page: ColumnLayout {
            clip: true
            spacing: 0
            Repeater {
                model: categoryModel
                delegate: ItemDelegate {
                    property string name: model.id
                    property bool canDelete: model.canDelete

                    text: model.display
                    icon.name: model.icon
                    checked: true
                    backgroundVisible: false
                    cascadeSelected: true
                    Layout.fillWidth: true
                    // checkable: false
                    indicatorVisible: model.isDefault

                    onClicked: {
                        if (!model.isDefault) {
                            categoryModel.setDefaultApp(model.id)
                        }
                    }
                    background: DccItemBackground {
                        separatorVisible: true
                        highlightEnable: false
                    }
                }
            }
        }
    }
}
