// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D
import org.deepin.dcc 1.0
import QtQuick.Window 2.15
import Qt.labs.platform

Rectangle {
    id: root
    property alias deviceModel: repeater.model
    property bool backgroundVisible: true
    property bool showMoreBtn: true
    property bool showPowerStatus: true
    property bool showConnectStatus: true
    signal clicked(int index, bool checked)

    color: "transparent"
    implicitHeight: layoutView.height
    Layout.fillWidth: true
    ColumnLayout {
        id: layoutView
        width: parent.width
        clip: true
        spacing: 0
        Repeater {
            id: repeater
            delegate: D.ItemDelegate {

                property bool showSendFile: model.canSendFile && model.connectStatus === 2
                // 用于保存移除的菜单项
                property Item removedItem: null
                id: itemCtl
                Layout.fillWidth: true
                Layout.fillHeight: true

                topPadding: 0
                bottomPadding: 0

                cascadeSelected: true
                visible: model.visiable
                backgroundVisible: root.backgroundVisible
                icon.name: model.iconName
                contentFlow: true
                hoverEnabled: true

                corners: getCornersForBackground(index, repeater.count)

                background: DccItemBackground {
                    separatorVisible: true
                }

                content: Rectangle {
                    width: parent.width
                    height: 50
                    color: "transparent"
                    MouseArea {
                        anchors.fill: parent
                        onDoubleClicked: {
                            console.log("双击了项 ", model.id, model.adapterId);
                            // 处理双击事件
                            if (model.connectStatus !== 2) {
                                dccData.work().connectDevice(model.id, model.adapterId)
                            }
                        }
                    }

                    RowLayout {
                        width: parent.width
                        anchors.fill: parent
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        ColumnLayout {
                            id: status
                            Layout.fillHeight: true
                            spacing: 0
                            Layout.leftMargin: 10
                            Label {
                                id: myDeviceName
                                text: model.name
                                font: D.DTK.fontManager.t6
                                horizontalAlignment: Qt.AlignLeft
                                verticalAlignment: Qt.AlignVCenter
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                                Layout.fillHeight: !showConnectStatus
                            }

                            Loader {
                                id: loader
                                active: showConnectStatus
                                sourceComponent:
                                    RowLayout {
                                    id: deatilShow
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    Label {
                                        id: nameDetail
                                        width: 60
                                        text: model.connectStatusText
                                        horizontalAlignment: Qt.AlignLeft
                                        verticalAlignment: Qt.AlignTop
                                        font.pointSize: 8
                                        Layout.fillHeight: true
                                    }

                                    D.DciIcon {
                                        id: powerIcon
                                        visible: model.battery !== "0%"
                                        width: 20
                                        name: model.batteryIconPath
                                        sourceSize: Qt.size(14, 14)
                                    }

                                    Label {
                                        id: power
                                        visible: model.battery !== "0%"
                                        text: model.battery
                                        horizontalAlignment: Qt.AlignLeft
                                        verticalAlignment: Qt.AlignTop
                                        font.pointSize: 8
                                    }
                                }
                                onLoaded: {
                                    item.show()
                                }
                            }
                        }

                        D.LineEdit {
                            id: nameEdit
                            visible: false
                            text: model.name
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.topMargin: 10
                            Layout.bottomMargin: 10

                            onEditingFinished: {
                                nameEdit.visible = false
                                status.visible = true
                                itemCtl.hoverEnabled = true

                                console.log("set device name ", model.id, nameEdit.text)
                                dccData.work().setDeviceAlias(model.id, nameEdit.text)

                            }
                            onVisibleChanged: {
                                if (visible) {
                                    text = model.name
                                }
                            }
                            Keys.onPressed: {
                                if (event.key === Qt.Key_Return) {
                                    hostNameEdit.forceActiveFocus(false); // 结束编辑
                                }
                            }
                        }

                        RowLayout {
                            id: rowCtl
                            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                            spacing: 10
                            D.BusyIndicator {
                                id: connectBusyIndicator
                                implicitHeight: 20
                                implicitWidth: 20
                                Layout.rightMargin: showMoreBtn ? 0 : 10
                                Layout.alignment: Qt.AlignVCenter
                                running: model.connectStatus === 1
                                visible: model.connectStatus === 1
                            }
                            D.ToolButton {
                                id: connectBtn
                                implicitHeight: 30
                                background.visible: true
                                text: model.connectStatus === 2 ? qsTr("Disconnect") : qsTr("Connect")
                                enabled: model.connectStatus === 2 || model.connectStatus === 0
                                font: D.DTK.fontManager.t6
                                visible: itemCtl.hovered && model.connectStatus !== 1

                                Layout.alignment: Qt.AlignVCenter
                                onClicked: {
                                    if (model.connectStatus === 2) {
                                        dccData.work().disconnectDevice(model.id)
                                    } else {
                                        dccData.work().connectDevice(model.id, model.adapterId)
                                    }
                                }
                            }

                            D.IconButton {
                                id: moreBtn
                                visible: showMoreBtn
                                Layout.alignment: Qt.AlignRight
                                icon.name: "bluetooth_option"
                                icon.width: 20
                                icon.height: 20

                                implicitHeight: 24
                                implicitWidth: 24

                                flat: !hovered

                                D.Menu {
                                    id: contextMenu
                                    implicitWidth: 150

                                    D.MenuItem {
                                        id: connectDev
                                        padding: 0
                                        text: model.connectStatus === 2 ? qsTr("Disconnect") : qsTr("Connect")
                                        enabled: model.connectStatus === 2 || model.connectStatus === 0
                                        onTriggered: {
                                            if (model.connectStatus === 2) {
                                                dccData.work().disconnectDevice(model.id)
                                            } else {
                                                dccData.work().connectDevice(model.id, model.adapterId)
                                            }
                                        }
                                    }
                                    D.MenuItem {
                                        id: sendFile
                                        padding: 0
                                        text: qsTr("Send Files")

                                        // TODO 当前通过visible属性隐藏的时候item隐藏了，但是外部的区域没有隐藏会有空白显示，暂时改成通过takeItem移除的方式
                                        // visible: model.canSendFile && model.connectStatus
                                        onTriggered: {
                                            fileDlg.open()
                                        }

                                    }

                                    D.MenuItem {
                                        id: rename
                                        text: qsTr("Rename")
                                        padding: 0
                                        onTriggered: {
                                            nameEdit.visible = true

                                            status.visible = false
                                            itemCtl.hoverEnabled = false

                                            nameEdit.forceActiveFocus(true)
                                            nameEdit.selectAll()
                                        }
                                    }

                                    D.MenuSeparator {
                                    }

                                    D.MenuItem {
                                        id: removeDev
                                        text: qsTr("Remove Device")
                                        enabled: model.connectStatus === 2 || model.connectStatus === 0
                                        padding: 0
                                        onTriggered: {
                                            dccData.work().ignoreDevice(model.id, model.adapterId)
                                        }
                                    }

                                    onOpened: {
                                        if (contextMenu.count == 5 && !showSendFile) {
                                            removedItem = contextMenu.takeItem(1)
                                            return
                                        }

                                        if (contextMenu.count == 4 && showSendFile) {
                                            contextMenu.insertItem(1, removedItem)
                                        }
                                    }
                                }

                                FileDialog {
                                    id: fileDlg
                                    title: qsTr("Select file")
                                    fileMode: FileDialog.OpenFiles
                                    folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
                                    onAccepted: {
                                        console.log(" Select file : ", fileDlg.files)
                                        dccData.work().showBluetoothTransDialog(model.address, fileDlg.files)

                                    }
                                }

                                onClicked: {
                                    console.log(" contextMenu 单击事件 ");
                                    // 在点击位置下方显示菜单
                                    // 获取按钮的全局位置，确保菜单在按钮的正下方显示
                                    var buttonGlobalX = moreBtn.x + moreBtn.width / 2 - contextMenu.width
                                    var buttonGlobalY = moreBtn.y + moreBtn.height + 5
                                    contextMenu.popup(buttonGlobalX, buttonGlobalY)
                                }
                            }
                        }

                    }
                }
            }
        }
    }
}
