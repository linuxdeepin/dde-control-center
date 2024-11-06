// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0

import org.deepin.dcc 1.0
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0

DccObject{
    DccObject {
        name: "OtherDeviceTitle"
        parentName: "otherDevice" + model.name
        displayName: qsTr("Other Devices")
        weight: 10
        hasBackground: false
        visible: model.powered
        pageType: DccObject.Item
        page: ColumnLayout {
            Label {
                Layout.leftMargin: 10
                font.bold: true
                font.pixelSize: 16
                text: dccObj.displayName
            }
        }
    }

    DccObject {
        name: "blueToothSwitch"
        parentName: "otherDevice" + model.name
        pageType: DccObject.Item
        weight: 20
        hasBackground: false
        visible: model.powered

        page: RowLayout {
            CheckBox {
                Layout.leftMargin: 10
                Layout.alignment: Qt.AlignLeft
                checked: dccData.model().displaySwitch
                text: qsTr("Show Bluetooth devices without names")
                onCheckedChanged: {
                    dccData.work().setDisplaySwitch(checked)
                }
            }

            IconButton {
                id: redobtn
                Layout.alignment: Qt.AlignRight
                flat: true
                visible: !model.discovering

                icon.name: "qrc:/icons/deepin/builtin/icons/bluetooth_redo.dci"
                onClicked: {
                    dccData.work().setAdapterDiscoverable(model.id)
                }
            }

            BusyIndicator {
                id: scanAnimation
                Layout.alignment: Qt.AlignRight
                running: model.discovering
                visible: model.discovering
                implicitWidth: 32
                implicitHeight: 32                
            }
        }

        Component.onCompleted: {
            console.log(" blueToothSwitch  checked changed")
            dccData.work().setAdapterDiscoverable(model.id)
        }
    }


    DccObject {
        name: "otherDeviceList"
        parentName: "otherDevice" + model.name
        weight: 40
        visible: model.powered
        hasBackground: true
        pageType: DccObject.Item
        page: BlueToothDeviceListView {
            showMoreBtn: false
            showConnectStatus: false
            showPowerStatus: false
            deviceModel: model.otherDevice

            onClicked: function (index, checked) {
            }
        }
    }
}
