//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    DccTitleObject {
        name: "taskBarTitle"
        weight: 500
        parentName: "personalization/dock"
        displayName: qsTr("Dock")
    }

    DccObject {
        name: "taskBarModeGroup"
        parentName: "personalization/dock"
        weight: 600
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "dockmode"
            parentName: "personalization/dock/taskBarModeGroup"
            displayName: qsTr("Mode")
            weight: 10
            pageType: DccObject.Item
            page: ColumnLayout {
                Label {
                    Layout.topMargin: 10
                    font: D.DTK.fontManager.t7
                    text: dccObj.displayName
                    Layout.leftMargin: 10
                }
                Flow {
                    id: modeLayout
                    spacing: 10
                    Layout.fillWidth: true
                    Layout.bottomMargin: 10
                    Layout.leftMargin: 10

                    ListModel {
                        id: modeData
                        ListElement { text: qsTr("Classic Mode"); icon: "effcient_left"; value: 1 }
                        ListElement { text: qsTr("Centered Mode"); icon: "effcient_center"; value: 0 }
                        // ListElement { text: qsTr("Fashion Mode"); icon: "fashion"; value: 2 }
                    }

                    Repeater {
                        model: modeData
                        ColumnLayout {
                            Item {
                                id: borderRect
                                Layout.preferredWidth: 144
                                Layout.preferredHeight: 70
                                Layout.alignment: Qt.AlignHCenter
                                Layout.margins: 0

                                Rectangle {
                                    anchors.fill: parent
                                    radius: 8
                                    color: "transparent"
                                    border.width: 2
                                    border.color: D.DTK.platformTheme.activeColor
                                    visible: dccData.dockInter.DisplayMode === model.value
                                }

                                Control {
                                    id: iconControl
                                    anchors.fill: parent
                                    anchors.margins: 4
                                    
                                    contentItem: D.DciIcon {
                                        palette: D.DTK.makeIconPalette(iconControl.palette)
                                        theme: iconControl.D.ColorSelector.controlTheme
                                        sourceSize: Qt.size(width, height)
                                        name: model.icon
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        dccData.dockInter.setDisplayMode(model.value)
                                    }
                                }
                            }

                            Text {
                                text: model.text
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font: D.DTK.fontManager.t9
                                color: dccData.dockInter.DisplayMode === model.value ? 
                                    D.DTK.platformTheme.activeColor : this.palette.windowText
                            }
                        }
                    }
                }
            }
        }
    }

    DccObject {
        name: "dockSettingsGroup"
        parentName: "personalization/dock"
        weight: 700
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "docksize"
            parentName: "personalization/dock/dockSettingsGroup"
            displayName: qsTr("Dock size")
            weight: 10
            pageType: DccObject.Editor
            page: RowLayout {
                spacing: 10
                Label {
                    Layout.alignment: Qt.AlignVCenter
                    font: D.DTK.fontManager.t7
                    text: qsTr("Small")
                }
                D.Slider {
                    Layout.alignment: Qt.AlignVCenter
                    id: balanceSlider
                    handleType: Slider.HandleType.ArrowBottom
                    implicitHeight: 24
                    highlightedPassedGroove: true
                    stepSize: 1
                    value: dccData.dockInter.DisplayMode === 2 ? dccData.dockInter.WindowSizeFashion : dccData.dockInter.WindowSizeEfficient 
                    from: 37
                    to: 100

                    onValueChanged: {
                        dccData.dockInter.resizeDock(value, true)
                    }

                    onPressedChanged: {
                        dccData.dockInter.resizeDock(value, pressed)
                    }
                }
                D.Label {
                    Layout.alignment: Qt.AlignVCenter
                    font: D.DTK.fontManager.t7
                    text: qsTr("Large")
                }
            }
        }
        DccObject {
            name: "lockedDock"
            parentName: "personalization/dock/dockSettingsGroup"
            displayName: qsTr("Lock the Dock")
            weight: 20
            pageType: DccObject.Editor
            page: Switch {
                checked: dccData.dockInter.locked 
                onCheckedChanged: {
                    if (dccData.dockInter.locked != checked)
                        dccData.dockInter.setLocked(checked)
                }
            }
        }
        DccObject {
            name: "positionInScreen"
            parentName: "personalization/dock/dockSettingsGroup"
            displayName: qsTr("Position on the screen")
            weight: 100
            pageType: DccObject.Editor
            page: CustomComBobox {
                flat: true
                model: alignModel
                currentIndex: indexByValue(dccData.dockInter.Position)

                ListModel {
                    id: alignModel
                    ListElement { text: qsTr("Top"); value: 0 }
                    ListElement { text: qsTr("Bottom"); value: 2 }
                    ListElement { text: qsTr("Left"); value: 3 }
                    ListElement { text: qsTr("Right"); value: 1 }
                }

                onCurrentIndexChanged: {
                    var selectedValue = model.get(currentIndex).value;
                    dccData.dockInter.setPosition(selectedValue)
                }
            }
        }

        DccObject {
            name: "positionInScreen"
            parentName: "personalization/dock/dockSettingsGroup"
            displayName: qsTr("Status")
            weight: 200
            pageType: DccObject.Editor
            page: CustomComBobox {
                flat: true
                model: hideModel
                currentIndex: indexByValue(dccData.dockInter.HideMode)

                ListModel {
                    id: hideModel
                    ListElement { text: qsTr("Keep shown"); value: 0 }
                    ListElement { text: qsTr("Keep hidden"); value: 1 }
                    ListElement { text: qsTr("Smart hide"); value: 2 }
                }

                onCurrentIndexChanged: {
                    var selectedValue = model.get(currentIndex).value;
                    dccData.dockInter.setHideMode(selectedValue)
                }
            }
        }
        DccObject {
            name: "combineApp"
            parentName: "personalization/dock/dockSettingsGroup"
            displayName: qsTr("Combine application icons")
            weight: 200
            pageType: DccObject.Editor
            page: Switch {
                checked: dccData.combineApp
                onCheckedChanged: {
                    if (dccData.combineApp !== checked)
                        dccData.setCombineApp(checked)
                }
            }
        }
    }

    DccObject {
        name: "multiscreenGroup"
        parentName: "personalization/dock"
        weight: 800
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "multiscreenItem"
            parentName: "personalization/dock/multiscreenGroup"
            displayName: qsTr("Multiple Displays")
            description: qsTr("Set the position of the taskbar on the screen")
            visible: dccData.displayMode === 2
            weight: 10
            pageType: DccObject.Editor
            page: CustomComBobox {
                flat: true
                model: showModeModel
                currentIndex: indexByValue(dccData.dockInter.showInPrimary)

                ListModel {
                    id: showModeModel
                    ListElement { text: qsTr("Only on main"); value: true }
                    ListElement { text: qsTr("On screen where the cursor is"); value: false }
                }

                onCurrentIndexChanged: {
                    var selectedValue = model.get(currentIndex).value;
                    dccData.dockInter.setShowInPrimary(selectedValue)
                }
            }
        }
    }

    DccObject {
        name: "pluginArea"
        weight:900
        icon: "plugin"
        parentName: "personalization/dock"
        displayName: qsTr("Plugin Area")
        description: qsTr("Select which icons appear in the Dock")

        PluginArea {}
    }
}
