// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import org.deepin.dcc 1.0

// 该文件中可以使用dccData,根对象为DccObject
DccObject {
    ExamplePage1 {
        name: "example_1"
        parentName: "example"
        displayName: qsTr("Normal Page")
        icon: "dcc_example"
        weight: 10
    }
    ExamplePage2 {
        name: "example_2"
        parentName: "example"
        displayName: qsTr("Settings Page")
        icon: "dcc_example"
        weight: 20
    }
    ExamplePage3 {
        name: "example_3"
        parentName: "example"
        displayName: qsTr("Settings Page")
        icon: "dcc_example"
        weight: 30
    }
}
