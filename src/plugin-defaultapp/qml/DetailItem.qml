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
    property CategoryModel categoryModel: null
    property bool canDelete: false
    page: DccRightView {
        isGroup: true
    }

    DccObject {
        name: "title"
        parentName: root.name
        displayName: qsTr("Please choose the default program to open '%1'").arg(root.displayName)
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
            // TODO: 添加删除默认程序暂不支持
            D.AbstractButton {
                Layout.alignment: Qt.AlignRight
                Layout.maximumWidth: 30
                Layout.maximumHeight: 30
                Layout.margins: 0
                enabled: canDelete
                icon.name: "action_reduce"
                onClicked: {
                    console.log(root.name, "-")
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
    DccRepeater {
        model: categoryModel
        delegate: DccObject {
            name: model.id
            parentName: root.name
            weight: 20 + index
            icon: model.icon
            displayName: model.display
            backgroundType: DccObject.ClickStyle
            pageType: DccObject.Editor
            page: RowLayout {
                DccCheckIcon {
                    visible: model.isDefault
                    mouseEnabled: false
                }
                D.ActionButton {
                    visible: !model.isDefault && model.canDelete
                    icon.name: "dcc-delete"
                    onClicked: {
                        categoryModel.removeApp(model.id)
                    }
                }
            }
            onActive: {
                if (!model.isDefault) {
                    categoryModel.setDefaultApp(model.id)
                }
            }
        }
    }
}
