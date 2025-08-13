// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0

import org.deepin.dcc 1.0
import QtQuick.Layouts 1.15

DccObject{
    property bool isUserClosingBluetooth: false  // 跟踪用户是否正在关闭蓝牙
    
    // 监听蓝牙状态变化，当真正开启时重置隐藏状态
    Connections {
        target: model
        function onPoweredChanged(poweredState, discoveringState) {
            if (model.powered && isUserClosingBluetooth) {
                // 蓝牙已真正开启，重置隐藏状态
                isUserClosingBluetooth = false;
            }
        }
    }
    
    DccObject {
        name: "blueToothCtl" + model.name
        parentName: "blueToothAdapters" + model.name + index
        weight: 10
        pageType: DccObject.Item
        page: DccGroupView {
            spacing: 0
            isGroup: false
        }
        BluetoothCtl{
            hideWhenUserClosing: isUserClosingBluetooth
            onUserClickedClose: {
                // 用户点击关闭时立即隐藏
                isUserClosingBluetooth = true
            }
            onUserClickedOpen: {
                // 用户点击开启时显示
                isUserClosingBluetooth = false
            }
        }
    }

    DccObject {
        name: "myDevice" + model.name
        parentName: "blueToothAdapters" + model.name+ index
        weight: 30
        pageType: DccObject.Item
        visible: model.powered && !isUserClosingBluetooth
        page: DccGroupView {
            spacing: 0
            isGroup: false
        }

        MyDevice{}
    }

    DccObject {
        name: "otherDevice" + model.name
        parentName: "blueToothAdapters" + model.name+ index
        weight: 40
        pageType: DccObject.Item
        visible: model.powered && !isUserClosingBluetooth
        page: DccGroupView {
            spacing: 0
            isGroup: false
        }

        OtherDevice{
            hideWhenUserClosing: isUserClosingBluetooth
        }
    }
}
