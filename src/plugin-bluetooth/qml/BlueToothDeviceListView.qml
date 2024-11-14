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
                leftPadding: 10
                implicitHeight: 60
                cascadeSelected: true
                visible: model.visiable
                backgroundVisible: root.backgroundVisible
                icon.name: model.iconName
                contentFlow: true

                hoverEnabled: true

                background: DccItemBackground {
                    separatorVisible: true
                    highlightEnable: false
                }

                content: Rectangle {

                    width: parent.width
                    height: 50

                    MouseArea {
                        anchors.fill: parent
                        onDoubleClicked: {
                            console.log("双击了项 ", model.id, model.adapterId);
                            //处理双击事件
                            if (model.connectStatus !== 2) {
                                dccData.work().connectDevice(model.id, model.adapterId)
                            }
                        }

                    }


                    RowLayout {
                        width: parent.width
                        Layout.fillWidth: true
                        Layout.fillHeight: true


                        Column {
                            id: status
                            Layout.alignment: Qt.AlignVCenter
                            Layout.leftMargin: 5
                            spacing: 0
                            height: myDeviceName.height + deatilShow.height
                            Label {
                                id: myDeviceName
                                height: 25
                                text: model.name
                                font: D.DTK.fontManager.t6
                                horizontalAlignment: Qt.AlignLeft
                                verticalAlignment: Qt.AlignBottom
                                leftPadding: 0
                                elide: Text.ElideRight
                                width: Math.min(implicitWidth, root.width - 200)
                            }

                            Row {
                                id: deatilShow
                                height: 25
                                visible: showConnectStatus
                                Label {
                                    id: nameDetail
                                    visible: showConnectStatus
                                    width: 60
                                    text: model.connectStatusText
                                    horizontalAlignment: Qt.AlignLeft
                                    verticalAlignment: Qt.AlignTop
                                    font.pointSize: 8
                                    color: "#5A000000"
                                }

                                D.DciIcon {
                                    id: powerIcon
                                    visible: showPowerStatus && model.battery !== "0%"
                                    width: 20
                                    name: model.batteryIconPath
                                    sourceSize: Qt.size(14, 14)
                                }

                                Label {
                                    id: power
                                    visible: showPowerStatus && model.battery !== "0%"
                                    text: model.battery
                                    horizontalAlignment: Qt.AlignLeft
                                    verticalAlignment: Qt.AlignTop
                                    font.pointSize: 8
                                    color: "#5A000000"
                                }

                            }

                        }

                        Rectangle {
                            id: nameEditBackgrd
                            width: root.width - 100
                            height: 50
                            color: "transparent"
                            visible: false
                            D.LineEdit {
                                id: nameEdit
                                visible: false
                                anchors.centerIn: parent
                                width: root.width - 100
                                height: 40
                                text: model.name
                                topPadding: 5
                                bottomPadding: 5

                                background: Rectangle {
                                    color: "transparent" // 设置为透明
                                    border.color: nameEdit.selectionColor // 边框颜色设置为蓝色
                                    border.width: 2      // 边框宽度
                                    radius: 5            // 可选：设置圆角
                                }

                                onEditingFinished: {
                                    nameEdit.visible = false
                                    nameEditBackgrd.visible = false
                                    status.visible = true
                                    itemCtl.hoverEnabled = true

                                    console.log("set device name ", model.id, nameEdit.text)
                                    dccData.work().setDeviceAlias(model.id, nameEdit.text)

                                }
                                Keys.onPressed: {
                                    if (event.key === Qt.Key_Return) {
                                        hostNameEdit.forceActiveFocus(false); // 结束编辑
                                    }
                                }
                            }
                        }

                        RowLayout {
                            id: rowCtl
                            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                            height: 50
                            spacing: 10
                            D.Button {
                                id: connectBtn
                                implicitWidth: 100
                                padding: 0
                                height: 30
                                background.visible: true
                                text: model.connectStatus === 2 ? qsTr("Disconnect") : qsTr("Connect")
                                enabled: model.connectStatus === 2 || model.connectStatus === 0
                                font: D.DTK.fontManager.t6
                                flat: false
                                visible: itemCtl.hovered
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
                                icon.name: "qrc:/icons/deepin/builtin/icons/bluetooth_option.dci"
                                icon.width: 16
                                icon.height: 16
                                width: 20
                                height: 20
                                flat: true

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
                                        text: qsTr("Send Files")

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
                                            nameEditBackgrd.visible = true

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
                                        text: qsTr("Remove Device")
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
                                    var buttonGlobalX = moreBtn.x + moreBtn.width / 2
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
