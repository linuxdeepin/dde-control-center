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
    Layout.preferredWidth: parent.width
    Layout.preferredHeight: parent.height

    color: "#f3f6f8"
    border.color: "white"
    border.width: 1

    width: parent.width
    height: parent.height

    readonly property int itemDelegateMinWidth: 204
    readonly property int itemDelegateMaxWidth: parent.width - 150
    readonly property int itemDelegateHeight: 20

    ColumnLayout {
        height: parent.height
        width: parent.width
        Layout.preferredHeight: parent.height

        Text {
            height: 40
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            text: qsTr("输出设备")
            Layout.leftMargin: 70
            Layout.topMargin: 20
            font: DTK.fontManager.t4
        }

        Text {
            height: 20
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            text: qsTr("选择是否启用设备")
            Layout.leftMargin: 70
            font: DTK.fontManager.t6
            opacity: 0.7
        }

        Rectangle {
            anchors {
                left: parent.left
                leftMargin: 60 // 设置左边间距为20
                rightMargin: 60
            }
            id: control
            color: "white"
            height: 160
            width: itemDelegateMaxWidth
            border.color: "white"
            border.width: 1
            radius: 10
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.preferredWidth: parent.width -120

            ListView {
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                implicitHeight: contentHeight
                width: parent.width - 40

                model: ListModel {
                    ListElement {
                        devName: "扬声器"
                        ischecked: true
                    }
                    ListElement {
                        devName: "蓝牙耳机1"
                        ischecked: false
                    }
                    ListElement {
                        devName: "蓝牙耳机2"
                        ischecked: false
                    }
                    ListElement {
                        devName: "蓝牙耳机3"
                        ischecked: false
                    }
                }

                delegate: ItemDelegate {
                    width: itemDelegateMaxWidth
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
                            width: control.width - 20
                            Layout.preferredHeight: 40
                            Layout.preferredWidth: control.width - 20

                            Label {
                                text: devName
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
                                    visible: ischecked
                                    // onClicked: {
                                    //     dccData.worker().setPausePlayer(checked)
                                    // }
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
                                    visible: !ischecked
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

        Text {
            Layout.alignment: Qt.AlignLeft
            text: qsTr("输入设备")
            Layout.leftMargin: 70
            font: DTK.fontManager.t4
        }

        Text {
            Layout.alignment: Qt.AlignLeft
            text: qsTr("选择是否启用设备")
            Layout.leftMargin: 70
            font: DTK.fontManager.t6
            opacity: 0.7
        }

        Rectangle {
            anchors {
                left: parent.left
                leftMargin: 60 // 设置左边间距为20
                rightMargin: 60
                topMargin: 30
            }
            id: control1
            color: "white"
            height: 200
            width: itemDelegateMaxWidth
            border.color: "white"
            border.width: 1
            radius: 10
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.preferredWidth: parent.width - 120
            Layout.topMargin: 10

            ListView {
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                implicitHeight: contentHeight
                width: parent.width - 40
                model: ListModel {
                    ListElement {
                        devName: "麦克风"
                        ischecked: true
                    }
                    ListElement {
                        devName: "麦克风1"
                        ischecked: false
                    }
                    ListElement {
                        devName: "麦克风2"
                        ischecked: false
                    }
                    ListElement {
                        devName: "蓝牙耳机3"
                        ischecked: false
                    }
                }

                delegate: ItemDelegate {
                    width: itemDelegateMaxWidth
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
                                text: devName
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
                                    visible: ischecked
                                    // onClicked: {
                                    //     dccData.worker().setPausePlayer(checked)
                                    // }
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
                                    visible: !ischecked
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
