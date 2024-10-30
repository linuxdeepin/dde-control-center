// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.0

import org.deepin.dcc 1.0
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0

DccObject{
    DccObject {
        name: "blueToothSwitch"
        parentName: "blueToothCtl" + model.name
        displayName: model.name
        pageType: DccObject.Item
        weight: 10
        hasBackground: true
        page: RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            height: 60
            id: root

            DciIcon {
                id: deviceIcon
                Layout.leftMargin: 10
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                name: "qrc:/icons/deepin/builtin/icons/bluetoothNomal"
                sourceSize:  Qt.size(36, 36)
            }

            Column {
                spacing: 0
                Label {
                    width: 100
                    id: myDeviceName
                    height: 25
                    text: dccObj.displayName
                    font: DTK.fontManager.t6
                    horizontalAlignment: Qt.AlignLeft
                    verticalAlignment: Qt.AlignBottom
                    leftPadding: 0
                }
                Label {
                    id: nameDetail
                    height: 25
                    width: root.width - deviceSwitch.width - 36 - 52
                    text: model.nameDetail
                    horizontalAlignment: Qt.AlignLeft
                    verticalAlignment: Qt.AlignTop
                    font.pointSize: 8
                    //opacity: 0.5
                    color:"#5A000000"
                    // 超链接点击事件
                    onLinkActivated: function(url) {
                        console.log("点击的链接是: " + url)
                        nameEdit.visible = true
                        nameDetail.visible = false
                        myDeviceName.visible = false
                        nameEditBackgrd.visible = true
                        nameEdit.forceActiveFocus(true)
                        nameEdit.selectAll()
                    }
                }

                Rectangle {
                    id: nameEditBackgrd
                    width: root.width - deviceSwitch.width - 36 - 52
                    height: 50
                    color: "transparent"
                    visible: false
                    LineEdit {
                        id: nameEdit
                        visible: false
                        anchors.centerIn: parent
                        width: root.width - deviceSwitch.width - 36 - 52
                        height: 30
                        text: myDeviceName.text
                        topPadding: 5
                        bottomPadding: 5

                        //maximumLength: 32

                        onTextChanged: {
                            if (text.length > 32) {
                                text = text.substr(0, 32);  // 截断到31个字符
                                nameEdit.alertText = qsTr("Length greater than or equal to 32")
                                nameEdit.showAlert = true
                            } else {
                                nameEdit.showAlert = false
                            }
                        }

                        // background: Rectangle {
                        //     color: "transparent" // 设置为透明
                        //     border.color: nameEdit.selectionColor // 边框颜色设置为蓝色
                        //     border.width: 2      // 边框宽度
                        //     radius: 5            // 可选：设置圆角
                        // }

                        onEditingFinished: {
                            nameEdit.visible = false
                            nameDetail.visible = true
                            myDeviceName.visible = true
                            nameEditBackgrd.visible = false

                            dccData.work().setAdapterAlias(model.id, nameEdit.text)
                        }
                        Keys.onPressed: {
                            if (event.key === Qt.Key_Return) {
                                nameEdit.forceActiveFocus(false); // 结束编辑
                            }
                        }
                    }
                }
            }

            BusyIndicator {
                id: initAnimation
                //    Layout.alignment: Qt.AlignRight
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
        hasBackground: false
        visible: !dccData.model().airplaneEnable && model.powered

        page: CheckBox {
            checked: model.discoverabled
            leftPadding: 15
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
        hasBackground: false
        visible: dccData.model().airplaneEnable
        page: Label {
            height: 25
            text: qsTr("To use the Bluetooth function, please turn off") + "<a href=\"飞行模式\">" + qsTr("Airplane Mode") +"</a>"
            horizontalAlignment: Qt.AlignLeft
            verticalAlignment: Qt.AlignTop
            font.pointSize: 8
            color:"#5A000000"
            // 超链接点击事件
            onLinkActivated: function(url) {
                console.log("点击的链接是: " + url)
                dccData.work().jumpToAirPlaneMode()
            }
        }
    }
}
