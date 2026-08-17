// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform
import Qt.labs.qmlmodels

import org.deepin.dtk as D
import org.deepin.dtk.style as DS
import org.deepin.dcc
import org.deepin.dcc.defApp

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
        page: Button {
            implicitWidth: implicitContentWidth + 20
            implicitHeight: 30
            text: qsTr("add")
            onClicked: {
                fileDialog.open()
            }
            onVisibleChanged: {
                fileDialog.close()
            }
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
            pageType: DccObject.Item
            page: D.ItemDelegate {
                id: control
                leftPadding: 10
                rightPadding: 8
                topPadding: 0
                bottomPadding: 0
                icon.name: dccObj.icon
                text: dccObj.displayName
                checked: false
                hoverEnabled: true
                cascadeSelected: false
                checkable: false
                onClicked: categoryModel.setDefaultApp(model.id)
                background: null
                implicitHeight: 48
                content: RowLayout {
                    width: 38
                    DccCheckIcon {
                        Layout.alignment: Qt.AlignCenter
                        visible: model.isDefault
                    }
                    D.IconButton {
                        Layout.alignment: Qt.AlignCenter
                        implicitHeight: 30
                        implicitWidth: 30
                        visible: !model.isDefault && model.canDelete && control.hovered
                        icon {
                            name: "dcc-delete"
                            width: 16
                            height: 16
                        }
                        onClicked: {
                            categoryModel.removeApp(model.id)
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
