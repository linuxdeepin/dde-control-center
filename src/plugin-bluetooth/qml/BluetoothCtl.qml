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
    signal userClickedClose()
    signal userClickedOpen()
    property bool hideWhenUserClosing: false  // 接收来自父组件的隐藏状态
    
    DccObject {
        name: "blueToothSwitch"
        parentName: "blueToothCtl" + model.name
        displayName: model.name
        pageType: DccObject.Item
        weight: 10
        backgroundType: DccObject.Normal
        page: RowLayout {
            id: root
            Layout.fillWidth: true
            spacing: 0

            property bool isSwitching: false

            Connections {
                target: model
                function onPoweredChanged(poweredState, discoveringState) {
                    if (!dccData.model().airplaneEnable) {
                        if (deviceSwitch.checked !== model.powered) {
                            deviceSwitch.checked = model.powered;
                        }
                    } else {
                        if (deviceSwitch.checked) {
                            deviceSwitch.checked = false;
                        }
                    }

                    if (deviceSwitch.checked === model.powered || dccData.model().airplaneEnable) {
                       if (isSwitching) {
                           if (model.powered && !dccData.model().airplaneEnable) {
                               dccData.work().setAdapterDiscovering(model.id, true);
                               dccData.work().setAdapterDiscoverable(model.id);
                           } else {
                               dccData.work().setAdapterDiscovering(model.id, false);
                           }
                       }
                        isSwitching = false;
                    } else {
                        isSwitching = true;
                    }
               }
            }

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
                    implicitHeight: Math.max(nameDetail.height, editBtn.height) + 5

                    Label {
                        id: nameDetail
                        anchors.left: parent.left
                        anchors.top: parent.top
                        width: Math.min(implicitWidth, parent.width - editBtn.implicitWidth - 10)
                        text: model.nameDetail
                        horizontalAlignment: Qt.AlignLeft
                        verticalAlignment: Text.AlignTop
                        font: DTK.fontManager.t10
                        elide: Text.ElideRight
                        wrapMode: Text.WordWrap
                        maximumLineCount: 2
                        height: Math.min(implicitHeight, 40)
                    }

                    ToolButton {
                        id: editBtn
                        anchors.left: nameDetail.right
                        anchors.verticalCenter: nameDetail.verticalCenter
                        font: DTK.fontManager.t10
                        text: qsTr("Edit")
                        background: null

                        hoverEnabled: true
                        implicitHeight: 20
                        textColor: Palette {
                            normal {
                                common: DTK.makeColor(Color.Highlight)
                            }
                            normalDark: normal
                            hovered {
                                common: DTK.makeColor(Color.Highlight).lightness(+10)
                            }
                            hoveredDark: hovered
                            pressed {
                                common: DTK.makeColor(Color.Highlight).lightness(-10)
                            }
                            pressedDark: pressed
                        }

                        onClicked: {
                            nameEdit.visible = true
                            devName.visible =false
                            nameEdit.forceActiveFocus(true)
                            nameEdit.selectAll()
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
                        nameEdit.alertText = qsTr("Bluetooth name cannot exceed 64 characters")
                        nameEdit.showAlert = true
                        dccData.work().playErrorSound()
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

                function updateAlias() {
                    if (nameEdit.text === "") {
                        nameEdit.text = myDeviceName.text
                    } else {
                        dccData.work().setAdapterAlias(model.id, nameEdit.text)
                    }
                    nameEdit.visible = false
                    devName.visible = true
                }

                onEditingFinished: {
                    updateAlias()
                }

                onFocusChanged: {
                    if (!focus) {
                        updateAlias()
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
                visible: isSwitching
                implicitWidth: 32
                implicitHeight: 32
            }

            Switch {
                id: deviceSwitch
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.rightMargin: 10

                enabled: !dccData.model().airplaneEnable && !isSwitching               
                checked: !dccData.model().airplaneEnable && model.powered

                Component.onCompleted: {
                    if (dccData.model().airplaneEnable) {
                        if (checked) checked = false;
                    } else {
                        if (checked !== model.powered) {
                            checked = model.powered;
                        }
                    }
                }

                Connections {
                    target: dccData.model()
                    function onAirplaneEnableChanged(airplaneModeEnabled) {
                        if (airplaneModeEnabled) {
                            checked = false;
                        } else {
                            checked = model.powered;
                        }
                    }
                }

                onClicked: {
                    if (enabled) {
                        isSwitching = true;

                        // 立即发送信号通知父组件
                        if (!checked) {
                            // 用户点击关闭蓝牙
                            userClickedClose();
                        } else {
                            // 用户点击开启蓝牙
                            userClickedOpen();
                        }

                        dccData.work().setAdapterPowered(model.id, checked);
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
        visible: !dccData.model().airplaneEnable && model.powered && !hideWhenUserClosing

        page: RowLayout {
            CheckBox {
                checked: model.discoverabled
                Layout.alignment: Qt.AlignLeft
                leftPadding: 10
                text: qsTr("Allow other Bluetooth devices to find this device")
                
                onCheckedChanged: {
                    dccData.work().setAdapterDiscoverable(model.id ,checked)
                }
            }
        }
    }

    DccObject {
        name: "airplaneModeTips"
        parentName: "blueToothCtl"  + model.name
        pageType: DccObject.Item
        weight: 30
        visible: dccData.model().airplaneEnable
        page: Item {
            height: 25

            Label {
                id: airplaneModeTextLabel
                text: qsTr("To use the Bluetooth function, please turn off")
                anchors.left: parent.left
                anchors.top: parent.top
                font.pointSize: 8
                width: implicitWidth
            }
            Label {
                id: airplaneModeLinkLabel
                text: "<a href=\"airplaneMode\" style=\"text-decoration:none;\">" + qsTr("Airplane Mode") +"</a>"
                anchors.left: airplaneModeTextLabel.right
                anchors.top: parent.top
                font.pointSize: 8
                textFormat: Text.RichText
                width: implicitWidth

                // 超链接点击事件
                onLinkActivated: function(url) {
                    console.log("点击的链接是: " + url)
                    dccData.work().jumpToAirPlaneMode()
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.ArrowCursor
                    acceptedButtons: Qt.NoButton

                    onEntered: {
                        cursorShape = Qt.PointingHandCursor;
                    }
                    onExited: {
                        cursorShape = Qt.ArrowCursor;
                    }
                }
            }
        }
    }
}
