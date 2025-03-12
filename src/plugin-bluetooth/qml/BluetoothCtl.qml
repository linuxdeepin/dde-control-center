// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.0

import org.deepin.dcc 1.0
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0
import org.deepin.dtk.style 1.0 as DS

DccObject{
    DccObject {
        name: "blueToothSwitch"
        parentName: "blueToothCtl" + model.name
        displayName: model.name
        pageType: DccObject.Item
        weight: 10
        backgroundType: DccObject.Normal
        page: RowLayout {
            Layout.fillWidth: true
            spacing: 0
            id: root
            DciIcon {
                id: deviceIcon
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                name: "bluetoothNomal"
                sourceSize: Qt.size(24, 24)
                Layout.preferredHeight: 50
                Layout.preferredWidth: 40
            }

            ColumnLayout {
                spacing: 0
                id: devName
                Layout.fillWidth: true
                Label {
                    id: myDeviceName
                    Layout.alignment: Qt.AlignLeft
                    text: dccObj.displayName
                    font: DTK.fontManager.t6
                    horizontalAlignment: Qt.AlignLeft
                    verticalAlignment: Qt.AlignBottom
                    leftPadding: 0
                    topPadding: 5
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Rectangle {
                    color: "transparent"
                    id: nameDetaillay
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    RowLayout {
                        spacing: 0
                        Layout.fillWidth: true
                        Label {
                            id: nameDetail
                            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                            width: Math.min(implicitWidth, nameDetaillay.width - editBtn.width - 10)

                            text: model.nameDetail
                            horizontalAlignment: Qt.AlignLeft
                            verticalAlignment: Qt.AlignTop
                            font.pointSize: 8
                            elide: Text.ElideRight
                        }

                        ToolButton {
                            id: editBtn
                            font.pointSize: 8
                            text: qsTr("Edit")

                            hoverEnabled: false
                            implicitHeight: 20
                            spacing: 0
                            textColor: DS.Style.highlightedButton.text
                            Layout.alignment: Qt.AlignTop
                            
                            onClicked: {
                                nameEdit.visible = true
                                devName.visible =false
                                nameEdit.forceActiveFocus(true)
                                nameEdit.selectAll()
                            }
                        }
                    }
                }


            }

            LineEdit {
                id: nameEdit
                visible: false
                Layout.fillWidth: true
              //  implicitWidth: root.width - deviceSwitch.width - 40 - 32
                text: myDeviceName.text
                topPadding: 5
                bottomPadding: 5
                Layout.rightMargin: 10
                implicitHeight: 30

                onTextChanged: {
                    if (text.length > 64) {
                        text = text.substr(0, 64);  // 截断到31个字符
                        nameEdit.alertText = qsTr("Length greater than or equal to 64")
                        nameEdit.showAlert = true
                        alertTimer.start()
                    } else {
                        nameEdit.showAlert = false
                    }
                }
                Timer {
                    id: alertTimer
                    interval: 2000
                    repeat: false
                    onTriggered: {
                        nameEdit.showAlert = false
                    }
                }

                onEditingFinished: {
                    nameEdit.visible = false
                    devName.visible = true
                    dccData.work().setAdapterAlias(model.id, nameEdit.text)
                }
                onFocusChanged: {
                    if (!focus) {
                        nameEdit.visible = false
                        devName.visible = true
                        dccData.work().setAdapterAlias(model.id, nameEdit.text)
                    }
                }

                Keys.onPressed: {
                    if (event.key === Qt.Key_Return) {
                        nameEdit.forceActiveFocus(false); // 结束编辑
                    }
                }
            }

            BusyIndicator {
                id: initAnimation
                running: true
                visible: false
                implicitWidth: 32
                implicitHeight: 32
            }

            Timer {
                id: timer
                interval: 6000  // 1000毫秒，即1秒
                repeat: false    // 设置为重复
                running: false  // 初始状态不运行
                onTriggered: {

                    initAnimation.visible = false
                    deviceSwitch.enabled = true
                }

            }

            Switch {
                id: deviceSwitch
                checked: model.powered
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.rightMargin: 10
                onCheckedChanged: {
                    if (checked === model.powered) {
                        return
                    }

                    dccData.work().setAdapterPowered(model.id ,checked)
                    deviceSwitch.enabled = false
                    timer.running = true
                    initAnimation.visible = true
                    dccData.work().setAdapterDiscovering(model.id, checked)

                    if (checked) {
                        dccData.work().setAdapterDiscoverable(model.id)
                    }
                }
            }

        }
    }

    DccObject {
        name: "blueToothSwitch"
        parentName: "blueToothCtl"  + model.name
        icon: "audio"
        pageType: DccObject.Item
        weight: 20
        visible: !dccData.model().airplaneEnable && model.powered

        page: CheckBox {
            checked: model.discoverabled
            leftPadding: 10
            text: qsTr("Allow other Bluetooth devices to find this device")
            
            onCheckedChanged: {
                dccData.work().setAdapterDiscoverable(model.id ,checked)
            }
        }
    }

    DccObject {
        name: "airplaneModeTips"
        parentName: "blueToothCtl"  + model.name
        pageType: DccObject.Item
        weight: 30
        visible: dccData.model().airplaneEnable
        page: Label {
            height: 25
            text: qsTr("To use the Bluetooth function, please turn off") + "<a href=\"飞行模式\">" + qsTr("Airplane Mode") +"</a>"
            horizontalAlignment: Qt.AlignLeft
            verticalAlignment: Qt.AlignTop
            font.pointSize: 8
            // 超链接点击事件
            onLinkActivated: function(url) {
                console.log("点击的链接是: " + url)
                dccData.work().jumpToAirPlaneMode()
            }
        }
    }
}
