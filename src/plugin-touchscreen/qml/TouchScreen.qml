// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

import org.deepin.dtk as D
import org.deepin.dtk.style as DS

import org.deepin.dcc
import org.deepin.dcc.touchscreen

DccObject {
    DccTitleObject {
        name: "TouchScreen"
        weight: 10
        parentName: "touchscreen"
        displayName: qsTr("TouchScreen")
    }

    DccObject {
        name: "TouchScreenTips"
        parentName: "touchscreen"
        pageType: DccObject.Item
        displayName: qsTr("Set up here when connecting the touch screen")
        page: ColumnLayout {
            DccLabel {
                Layout.fillWidth: true
                Layout.leftMargin: 12
                font: D.DTK.fontManager.t8
                text: dccObj.displayName
            }
        }
    }

    DccObject {
        parentName: "touchscreen"
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: ColumnLayout {
            Repeater {
                model: dccData.touchScreenMatchModel()
                delegate: ItemDelegate{
                    id: touchItem
                    property var data: model
                    text: model.name
                    checkable: false
                    Layout.fillWidth: true
                    Layout.leftMargin: 8
                    contentItem: RowLayout {
                        spacing: 8
                        DccLabel {
                            text: touchItem.text
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }
                        D.ComboBox{
                            id: combo
                            flat: true
                            model: dccData.monitors
                            currentIndex: dccData.monitors.indexOf(touchItem.data.screenName)
                            onCurrentIndexChanged: {
                                if (touchItem.data.screenName !== dccData.monitors[currentIndex]) {
                                    dccData.assoiateTouch(dccData.monitors[currentIndex],touchItem.data.UUID)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
