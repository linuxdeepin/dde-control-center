// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0

DccObject {
    id: root
    property real calcValue
    property real calcType: 0
    DccObject {
        name: "calc"
        parentName: root.name
        displayName: qsTr("calc")
        icon: "dcc_example"
        weight: 10
        backgroundType: DccObject.Normal // 设置背景样式
        pageType: DccObject.Editor // Editor为page是右边的控件，左边显示displayName、icon等
        page: Button {
            text: dccObj.displayName
            onClicked: {
                calcValue = dccData.calc(calcValue, 2)
            }
        }
    }
    DccObject {
        name: "value"
        parentName: root.name
        displayName: qsTr("value")
        weight: 20
        pageType: DccObject.Editor
        backgroundType: DccObject.ClickStyle // ClickStyl表示有点击效果，点击时会发出active信号
        page: RowLayout {
            Text {
                text: calcValue
            }
            ComboBox {}
        }
        onActive: cmd => console.log(this, "onActive:", cmd)
    }
    DccObject {
        name: "group"
        parentName: root.name
        displayName: qsTr("group")
        weight: 30
        pageType: DccObject.Item
        page: DccGroupView {} // 组效果，其子项会放在一个组里
        DccObject {
            name: "item2" // name要求当前组内唯一
            parentName: root.name + "/group" // parentName要求可定位到对应项，可用多个DccObject的name组合
            displayName: qsTr("value")
            weight: 20
            pageType: DccObject.Item // Item的page将占整个区域
            page: Text {
                text: calcValue
            }
        }
        DccObject {
            name: "item1"
            parentName: root.name + "/group"
            displayName: qsTr("value")
            weight: 10
            pageType: DccObject.Editor
            page: Text {
                text: calcValue
            }
        }
        DccObject {
            id: calcTypeObj
            name: "calcType"
            parentName: root.name
            displayName: qsTr("calc type")
            description: qsTr("description")
            icon: "dcc_example"
            weight: 30
            backgroundType: DccObject.Normal
            pageType: DccObject.Editor
            page: Button {
                text: dccObj.displayName
                onClicked: {
                    calcType++
                    if (calcType >= 4) {
                        calcType = 0
                    }
                    dccData.setCalcType(calcType)
                }
            }
            Connections {
                target: dccData
                function onCalcTypeChanged(cType) {
                    calcTypeObj.displayName = cType
                }
            }
        }
    }

    DccObject {
        name: "group2"
        parentName: root.name
        displayName: qsTr("group2")
        weight: 40
        pageType: DccObject.Item
        page: DccGroupView {}
        DccRepeater { // DccRepeater配合model可实现多个DccObject
            model: 3
            delegate: DccObject {
                name: "item" + (index + 1)
                parentName: root.name + "/group2"
                displayName: qsTr("Item") + (index + 1)
                weight: 30 + index
                pageType: DccObject.Editor
                page: Switch {}
            }
        }
    }
}
