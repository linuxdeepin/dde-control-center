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
            id: root
            Layout.fillWidth: true
            spacing: 0

            property bool isSwitching: false

            Component.onCompleted: {
                deviceSwitch.checked = model.powered
            }

            Connections {
                target: model
                function onPoweredChanged(poweredState, discoveringState) {
                    if (deviceSwitch.checked === model.powered) {
                        isSwitching = false;
                        deviceSwitch.enabled = true;
                    } else {
                        isSwitching = true;
                        deviceSwitch.enabled = false;
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
                    Layout.fillHeight: true

                    Label {
                        id: nameDetail
                        anchors.left: parent.left
                        anchors.top: parent.top
                        width: Math.min(implicitWidth, parent.width - editBtn.implicitWidth - 10)
                        text: model.nameDetail
                        horizontalAlignment: Qt.AlignLeft
                        verticalAlignment: Text.AlignTop
                        font.pointSize: 8
                        elide: Text.ElideRight
                    }

                    ToolButton {
                        id: editBtn
                        anchors.left: nameDetail.right
                        anchors.top: nameDetail.top
                        font.pointSize: 8
                        text: qsTr("Edit")

                        hoverEnabled: false
                        implicitHeight: 20
                        textColor: DS.Style.highlightedButton.text

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
                        nameEdit.alertText = qsTr("Length greater than or equal to 64")
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
                    if (nameEdit.text.trim() === "") {
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
                onCheckedChanged: {
                    enabled = false;
                    isSwitching = true;
                    dccData.work().setAdapterPowered(model.id ,checked);
                    if (checked) {
                        dccData.work().setAdapterDiscovering(model.id, true);
                        dccData.work().setAdapterDiscoverable(model.id);
                    } else {
                        dccData.work().setAdapterDiscovering(model.id, false);
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
