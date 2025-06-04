// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls 2.3
import QtQuick.Layouts
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    // 键盘布局列表抬头
    DccObject {
        id: keyboardLayoutTitle
        property bool isEditing: false
        name: "KeyboardLayoutTitle"
        parentName: "KeyboardLayout"
        displayName: qsTr("Keyboard layout")
        weight: 10
        pageType: DccObject.Item
        page: RowLayout {
            Label {
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.leftMargin: 10
                font.bold: true
                font.pointSize: 14
                text: dccObj.displayName
            }
            D.Button {
                id: button
                checkable: true
                visible: dccData.layoutCount > 1
                checked: keyboardLayoutTitle.isEditing
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.rightMargin: 10
                text: keyboardLayoutTitle.isEditing ? qsTr("done") : qsTr("edit")
                font.pointSize: 12
                background: null
                textColor: D.Palette {
                    normal {
                        common: D.DTK.makeColor(D.Color.Highlight)
                        crystal: D.DTK.makeColor(D.Color.Highlight)
                    }
                }
                onCheckedChanged: {
                    keyboardLayoutTitle.isEditing = button.checked
                }
            }
        }
    }

    DccObject {
        name: "KeyboardLayoutTips"
        parentName: "KeyboardLayout"
        weight: 12
        pageType: DccObject.Item
        page: D.Label {
            textFormat: Text.RichText
            text: qsTr("Add the corresponding input method in <a style='text-decoration: none;' href='Manage Input Methods'>Input Method Management</a> to ensure the keyboard layout works when added or switched.")
            leftPadding: 10
            rightPadding: 10
            wrapMode: Text.WordWrap
            onLinkActivated: link => DccApp.showPage(link)
        }
    }

    DccObject {
        name: "KeyboardLayoutGroup"
        parentName: "KeyboardLayout"
        weight: 20
        pageType: DccObject.Item
        page: DccGroupView {}

        DccRepeater {
            model: dccData.layoutCount
            delegate: DccObject {
                name: "KeyboardLayoutItem" + index
                parentName: "KeyboardLayoutGroup"
                weight: 10 + index * 2
                pageType: DccObject.Item
                backgroundType: DccObject.Normal
                page: ItemDelegate {
                    id: itemDelegate
                    property bool isCurrentLang: dccData.currentLayout === dccData.userLayoutAt(index, true)
                    visible: dccObj
                    hoverEnabled: true
                    implicitHeight: 40
                    // icon.name: dccObj.icon
                    checkable: false

                    Label {
                        id: itemName
                        text: dccData.userLayoutAt(index, true)
                        elide: Text.ElideRight
                        anchors {
                            left: itemDelegate.left
                            leftMargin: 10
                            top: itemDelegate.top
                            topMargin: (itemDelegate.height - itemName.height) / 2
                        }
                    }

                    D.IconButton {
                        id: removeButton
                        visible: itemDelegate.isCurrentLang || keyboardLayoutTitle.isEditing
                        icon.name: itemDelegate.isCurrentLang ? "item_checked" : "list_delete"
                        icon.width: 24
                        icon.height: 24
                        implicitWidth: 36
                        implicitHeight: 36
                        anchors {
                            right: itemDelegate.right
                            rightMargin: 10
                            top: itemDelegate.top
                            topMargin: (itemDelegate.height - removeButton.height) / 2
                        }
                        background: null
                        onClicked: {
                            if (!keyboardLayoutTitle.isEditing || itemDelegate.isCurrentLang)
                                return

                            dccData.deleteUserLayout(itemName.text)
                        }
                    }

                    background: DccItemBackground {
                        separatorVisible: true
                    }

                    onClicked: {
                        if (keyboardLayoutTitle.isEditing)
                            return

                        // console.log("set current layout", itemName.text, dccData.userLayoutAt(index, false))
                        dccData.currentLayout = dccData.userLayoutAt(index, false)
                    }
                }
            }
        }

        DccObject {
            name: "KeyboardLayoutLastItem"
            parentName: "KeyboardLayoutGroup"
            displayName: qsTr("Add new keyboard layout...")
            weight: 999
            pageType: DccObject.Item
            backgroundType: DccObject.Normal
            page: Item {
                implicitHeight: 40
                D.Button {
                    id: addButton
                    implicitHeight: 40
                    text: dccObj.displayName
                    background: null
                    textColor: D.Palette {
                        normal {
                            common: D.DTK.makeColor(D.Color.Highlight)
                            crystal: D.DTK.makeColor(D.Color.Highlight)
                        }
                    }
                    LayoutsChooser {
                        id: layoutsChooser
                        viewModel: dccData.layoutSearchModel()
                        onSelected: function (data) {
                            // console.log("selected data...", data)
                            dccData.addUserLayout(data)
                        }
                    }

                    onClicked: {
                        keyboardLayoutTitle.isEditing = false
                        layoutsChooser.active = true
                    }
                }
            }
        }
    }
}
