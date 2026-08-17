// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0

import org.deepin.dcc 1.0
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

DccObject{
    property bool refreshEnable: true
    property bool lastPoweredState: false  // 记录上次蓝牙开关状态
    property bool hideWhenUserClosing: false  // 接收来自父组件的隐藏状态

    Connections {
        target: DccApp
        function onActiveObjectChanged(object) {
           if (object.name === "bluetooth") {
                refreshEnable = true
                // 初始化状态记录
                lastPoweredState = model.powered
                if (model.powered && !model.discovering) {
                    deferDiscoverTimer.restart()
                }
           }
        }
    }

    // 监听蓝牙开关状态变化，确保重新开启时能立即启动扫描
    Connections {
        target: model
        function onPoweredChanged(poweredState, discoveringState) {
            if (model.powered && !dccData.model().airplaneEnable) {
                // 蓝牙开启时，重置刷新状态
                refreshEnable = true
                
                // 只有当蓝牙从关闭变为开启时才立即启动扫描
                if (!lastPoweredState && !model.discovering) {
                    dccData.work().setAdapterDiscovering(model.id, true);
                    dccData.work().setAdapterDiscoverable(model.id);
                }
            }
            
            // 更新状态记录
            lastPoweredState = model.powered
        }
    }

    Timer {
        id: autoRefreshTimer
        interval: 5000
        repeat: false
        onTriggered: {
            if (model.powered && refreshEnable && !model.discovering) {
                dccData.work().setAdapterDiscoverable(model.id)
            }
        }
    }

    Timer {
        id: deferDiscoverTimer
        interval: 300
        repeat: false
        onTriggered: {
            if (model.powered && !model.discovering) {
                dccData.work().setAdapterDiscoverable(model.id)
            }
        }
    }

    DccTitleObject {
        name: "OtherDeviceTitle"
        parentName: "otherDevice" + model.id
        displayName: qsTr("Other Devices")
        weight: 10
        visible: model.powered
    }

    DccObject {
        name: "blueToothSwitch"
        parentName: "otherDevice" + model.id
        pageType: DccObject.Item
        weight: 20
        visible: model.powered && !hideWhenUserClosing

        page: RowLayout {
            D.CheckBox {
                Layout.leftMargin: 10
                Layout.alignment: Qt.AlignLeft
                checked: dccData.model().displaySwitch
                text: qsTr("Show Bluetooth devices without names")
                onCheckedChanged: {
                    dccData.work().setDisplaySwitch(checked)
                }
            }

            D.ActionButton {
                id: redobtn
                Layout.alignment: Qt.AlignRight
                visible: !model.discovering
                palette.windowText: D.ColorSelector.textColor
                icon.name: "redo"
                icon.width: 16
                icon.height: 16
                implicitHeight: 30
                implicitWidth: 30
                flat: !hovered

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

                onClicked: {
                    dccData.work().setAdapterDiscoverable(model.id)
                }
            }

            D.BusyIndicator {
                id: scanAnimation
                Layout.alignment: Qt.AlignRight
                running: model.discovering
                visible: model.discovering
                implicitWidth: 20
                implicitHeight: 20
            }
        }

        Component.onCompleted: {
            if (typeof DccApp !== 'undefined' && DccApp.activeObject && DccApp.activeObject.name === "bluetooth") {
                if (model.powered && !model.discovering) {
                    deferDiscoverTimer.restart()
                }
            }
        }
    }

    Connections {
        target: model
        function onDiscoveringChanged() {
            if (!model.discovering && model.powered && refreshEnable) {
                autoRefreshTimer.restart()
            } else if (model.discovering) {
                // 正在扫描时停止定时器
                autoRefreshTimer.stop()
            }
        }
    }

    DccObject {
        name: "otherDeviceList"
        parentName: "otherDevice" + model.id
        weight: 40
        visible: model.powered
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: ColumnLayout {
            spacing: 0
            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 2
            }
            BlueToothDeviceListView {
                id: deviceListView
                Layout.fillWidth: true
                showMoreBtn: false
                showConnectStatus: false
                showPowerStatus: false

                Timer {
                    interval: 300
                    repeat: false
                    running: true
                    onTriggered: {
                        deviceListView.deviceModel = model.otherDevice
                    }
                }

                onClicked: function (index, checked) {
                }

                onVisibleChanged: {
                    refreshEnable = visible
                }
            }
        }
    }
}
