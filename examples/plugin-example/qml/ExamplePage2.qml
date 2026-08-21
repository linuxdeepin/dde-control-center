// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D

import org.deepin.dcc 1.0

DccObject {
    id: root
    property real calcValue
    property real calcType: 0

    weight: 10
    page: DccSettingsView {} // 设置界面，此DccObject的pageType为Menu，page为DccSettingsView
    DccObject {
        name: "body" // DccSettingsView里的限制，其子项必须是两个DccObject，第一个为主界面，第二个为下方按钮区域
        parentName: root.name
        weight: 10
        pageType: DccObject.Item
        DccObject {
            name: "calc"
            parentName: root.name + "/body"
            displayName: qsTr("calc")
            icon: "dcc_example"
            weight: 10
            backgroundType: DccObject.Normal
            pageType: DccObject.Editor
            page: Button {
                text: dccObj.displayName
                onClicked: {
                    calcValue = dccData.calc(calcValue, 2)
                }
            }
        }
        DccObject {
            name: "value"
            parentName: root.name + "/body"
            displayName: qsTr("no Search")
            canSearch: false // 设置界面通常不搜索，可设置canSearch
            weight: 20
            pageType: DccObject.Item
            page: Text {
                text: calcValue
            }
        }
        DccObject {
            name: "menuEditor"
            parentName: root.name + "/body"
            displayName: qsTr("no Search")
            canSearch: false // 设置界面通常不搜索，可设置canSearch
            weight: 30
            pageType: DccObject.MenuEditor // 菜单加编辑控件,子项是一个菜单项
            page: Switch {}
            DccObject {
                name: "menu"
                parentName: root.name + "/body/menuEditor"
                weight: 10
                DccRepeater {
                    model: 8
                    delegate: DccObject {
                        name: "item" + (index + 1)
                        parentName: "menuEditor/menu"
                        displayName: qsTr("Item") + (index + 1)
                        weight: 30 + index
                        backgroundType: DccObject.Normal
                        pageType: DccObject.Editor
                        page: Switch {}
                    }
                }
            }
        }
        DccObject {
            name: "calcType" // 该DccObject会显示在example_2中
            parentName: "example_2/body" // DccObject位置只与parentName和weight有关，与其自身位置无关
            weight: 80
            displayName: qsTr("calc type")
            pageType: DccObject.Editor
            backgroundType: DccObject.Normal
            page: Text {
                text: calcType
            }
        }
        DccObject {
            name: "group2"
            parentName: root.name + "/body"
            displayName: qsTr("group2")
            weight: 100
            pageType: DccObject.Item
            page: DccGroupView {}

            DccObject {
                name: "item0"
                parentName: root.name + "/body/group2"
                displayName: qsTr("value")
                weight: 20
                pageType: DccObject.Item
                page: Rectangle {
                    implicitHeight: 50
                    Text {
                        anchors.centerIn: parent
                        text: calcValue
                    }
                }
            }
            // DccRepeater类可配合model实现多个DccObject
            DccRepeater {
                model: 23
                delegate: DccObject {
                    name: "item" + (index + 1)
                    parentName: root.name + "/body/group2"
                    displayName: qsTr("Item") + (index + 1)
                    weight: 30 + index
                    pageType: DccObject.Editor
                    page: Switch {}
                }
            }
        }
    }
    DccObject {
        name: "footer"
        parentName: root.name
        weight: 20
        pageType: DccObject.Item
        DccObject {
            name: "delete"
            parentName: root.name + "/footer"
            weight: 10
            pageType: DccObject.Item
            page: Button {
                text: qsTr("Delete")
                onClicked: {
                    deleteDialog.createObject(this).show()
                }
            }
        }

        Component {
            id: deleteDialog
            D.DialogWindow {
                modality: Qt.ApplicationModal
                width: 380
                icon: "preferences-system"
                onClosing: destroy(10)
                ColumnLayout {
                    width: parent.width
                    Label {
                        Layout.fillWidth: true
                        Layout.leftMargin: 50
                        Layout.rightMargin: 50
                        text: qsTr("Are you sure you want to delete this configuration?")
                        font.bold: true
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignHCenter
                    }
                    RowLayout {
                        Layout.topMargin: 10
                        Layout.bottomMargin: 10
                        Button {
                            Layout.fillWidth: true
                            text: qsTr("Cancel")
                            onClicked: close()
                        }
                        Rectangle {
                            implicitWidth: 2
                            Layout.fillHeight: true
                            color: this.palette.button
                        }

                        Button {
                            Layout.fillWidth: true
                            text: qsTr("Delete")
                            onClicked: {
                                close()
                            }
                        }
                    }
                }
            }
        }
        DccObject {
            // 按钮区域可加个空项处理右对齐问题
            name: "spacer"
            parentName: root.name + "/footer"
            weight: 20
            pageType: DccObject.Item
            page: Item {
                Layout.fillWidth: true
            }
        }
        DccObject {
            name: "cancel"
            parentName: root.name + "/footer"
            weight: 30
            pageType: DccObject.Item
            page: Button {
                text: qsTr("Cancel")
                onClicked: {
                    DccApp.showPage(root.parentName)
                }
            }
        }
        DccObject {
            name: "save"
            parentName: root.name + "/footer"
            weight: 40
            pageType: DccObject.Item
            page: Button {
                text: qsTr("Save")
                onClicked: {
                    calcValue = dccData.calc(calcValue, 3)
                }
            }
        }
        function parseP(msgtext) {
            const param_reg = /<([^\]]+)>/
            const parameters = []
            let match = msgtext ? msgtext.match(param_reg) : null
            if (match) {
                const param = match[1].split(",").map(p => p.trim())
                for (let par of param) {
                    let num = parseInt(par, 10)
                    if (!isNaN(num)) {
                        parameters.push(num)
                        // parameters.push(par)
                    }
                }
            }
            return parameters
        }
    }
}
