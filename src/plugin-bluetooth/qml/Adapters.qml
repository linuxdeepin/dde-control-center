// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0

import org.deepin.dcc 1.0
import QtQuick.Layouts 1.15

DccObject{
    DccObject {
        name: "blueToothCtl" + model.name
        parentName: "blueToothAdapters" + model.name
        weight: 10
        pageType: DccObject.Item
        page: DccGroupView {
            spacing: 0
            isGroup: false
        }
        BluetoothCtl{}
    }

    DccObject {
        name: "myDevice" + model.name
        parentName: "blueToothAdapters" + model.name
        weight: 30
        pageType: DccObject.Item
        page: DccGroupView {
            spacing: 0
            isGroup: false
        }

        MyDevice{}
    }

    DccObject {
        name: "otherDevice" + model.name
        parentName: "blueToothAdapters" + model.name
        weight: 40
        pageType: DccObject.Item
        page: DccGroupView {
            spacing: 0
            isGroup: false
        }

        OtherDevice{}       
    }
}
