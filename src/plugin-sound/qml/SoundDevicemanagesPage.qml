// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import org.deepin.dtk 1.0

import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {

    readonly property int itemDelegateMaxWidth: parent.width
    anchors {
        left: parent.left
        top: parent.top
        topMargin: 10
    }
    color: "transparent"
    width: itemDelegateMaxWidth
    implicitHeight: controlAll.implicitHeight
    ColumnLayout {
        id: controlAll
        width: parent.width
        spacing: 5
        Text {
            height: 40
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            text: qsTr("输出设备")
            Layout.topMargin: 20
            font: DTK.fontManager.t4
        }

        Text {
            height: 20
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            text: qsTr("选择是否启用设备")
            font: DTK.fontManager.t6
            opacity: 0.7
        }

        Item {
            width: parent.width
            height: 5
        }

        Rectangle {
            anchors {
                left: parent.left
            }
            id: control
            color: "white"
            implicitHeight: 40 * outListView.count
            width: parent.width
            border.color: "white"
            border.width: 1
            radius: 10
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.preferredWidth: parent.width
            visible: outListView.count > 0

            ListView {
                id: outListView
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                implicitHeight: 40 * outListView.count
                implicitWidth: parent.width
                model: dccData.model().soundOutputDeviceModel()
                delegate: ItemDelegate {
                    implicitWidth: outListView.implicitWidth
                    implicitHeight: 40
                    backgroundVisible: false
                    corners: getCornersForBackground(index,
                        ListView.view.count)
                    contentFlow: true
                    cascadeSelected: true
                    content: ColumnLayout {
                        width: control.width
                        implicitHeight: 40
                        RowLayout {
                            implicitHeight: 35
                            width: control.width - 20
                            //    Layout.preferredHeight: 40
                            Layout.preferredWidth: control.width - 20

                            Label {
                                text: model.name
                                Layout.alignment: Qt.AlignLeft
                            }

                            RowLayout {
                                Layout.alignment: Qt.AlignRight
                                IconButton {
                                    icon.name: "qrc:/icons/deepin/builtin/actions/checked.png"
                                    icon.width: 24
                                    icon.height: 24
                                    implicitWidth: 24
                                    background: Rectangle {
                                        color: "transparent" // 背景透明
                                        border.color: "transparent" // 边框透明
                                        border.width: 0
                                    }
                                    visible: model.isChecked
                                    onClicked: {
                                        dccData.worker().setPortEnableIndex(index, false, 1)
                                    }
                                }
                                IconButton {
                                    icon.name: "qrc:/icons/deepin/builtin/actions/nocheck.png"
                                    icon.width: 24
                                    icon.height: 24
                                    implicitWidth: 24
                                    background: Rectangle {
                                        color: "transparent" // 背景透明
                                        border.color: "transparent" // 边框透明
                                        border.width: 0
                                    }
                                    visible: !model.isChecked
                                    onClicked: {
                                        dccData.worker().setPortEnableIndex(index, true, 1)
                                    }
                                }
                            }
                        }

                        Rectangle {
                            color: "#f2f2f2"
                            height: 1
                            Layout.preferredWidth: control.width - 20
                        }
                    }
                }
            }

        }

        Item {
            width: parent.width
            height: 5
        }

        Text {
            Layout.alignment: Qt.AlignLeft
            text: qsTr("输入设备")
            font: DTK.fontManager.t4
        }

        Text {
            Layout.alignment: Qt.AlignLeft
            text: qsTr("选择是否启用设备")
            font: DTK.fontManager.t6
            opacity: 0.7
        }

        Rectangle {
            anchors {
                left: parent.left
                topMargin: 30
            }
            id: control1
            color: "white"
            implicitHeight: 40 * inputListView.count
            width: itemDelegateMaxWidth
            border.color: "white"
            border.width: 1
            radius: 10
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.preferredWidth: itemDelegateMaxWidth
            Layout.topMargin: 10
            visible: inputListView.count > 0

            ListView {
                id: inputListView
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                implicitHeight: 40 * dccData.model().getDeviceManagerRowCount(1)
                width: parent.width - 40
                model: dccData.model().soundInputDeviceModel()

                delegate: ItemDelegate {
                    width: inputListView.width
                    height: 40
                    backgroundVisible: false
                    corners: getCornersForBackground(index,
                        ListView.view.count)
                    contentFlow: true
                    cascadeSelected: true
                    content: ColumnLayout {
                        width: control.width
                        Layout.preferredHeight: 40

                        RowLayout {
                            spacing: 5
                            width: control.width - 20
                            Layout.preferredHeight: 40
                            Layout.preferredWidth: control.width - 20

                            Label {
                                text: model.name
                                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            }

                            RowLayout {
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                IconButton {
                                    icon.name: "qrc:/icons/deepin/builtin/actions/checked.png"
                                    icon.width: 24
                                    icon.height: 24
                                    implicitWidth: 24
                                    background: Rectangle {
                                        color: "transparent" // 背景透明
                                        border.color: "transparent" // 边框透明
                                        border.width: 0
                                    }
                                    visible: model.isChecked
                                    onClicked: {
                                        dccData.worker().setPortEnableIndex(index, false, 2)
                                    }
                                }
                                IconButton {
                                    icon.name: "qrc:/icons/deepin/builtin/actions/nocheck.png"
                                    icon.width: 24
                                    icon.height: 24
                                    implicitWidth: 24
                                    background: Rectangle {
                                        color: "transparent" // 背景透明
                                        border.color: "transparent" // 边框透明
                                        border.width: 0
                                    }
                                    visible: !model.isChecked
                                    onClicked: {
                                        dccData.worker().setPortEnableIndex(index, true, 2)
                                    }
                                }
                            }
                        }

                        Rectangle {
                            color: "#f2f2f2"
                            height: 1
                            Layout.preferredWidth: control1.width - 20
                        }
                    }
                }
            }
        }
    }
}
