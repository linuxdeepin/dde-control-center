// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    DccTitleObject {
        name: "interfaceAndEffectTitle"
        parentName: "personalization/windowEffect"
        displayName: qsTr("Interface and Effects")
        visible: dccData.platformName() !== "wayland"
        weight: 10
    }

    DccObject {
        name: "interfaceAndEffect"
        parentName: "personalization/windowEffect"
        weight: 100
        pageType: DccObject.Item
        visible: dccData.platformName() !== "wayland"
        page: DccGroupView {}
        InterfaceEffectListview {}
    }

    DccTitleObject {
        name: "windowSettingsTitle"
        parentName: "personalization/windowEffect"
        displayName: qsTr("Window Settings")
        weight: 200
    }

    DccObject {
        name: "windowSettingsGroup"
        parentName: "personalization/windowEffect"
        weight: 300
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "roundedEffect"
            parentName: "personalization/windowEffect/windowSettingsGroup"
            displayName: qsTr("Window rounded corners")
            visible: dccData.model.windowEffectType < InterfaceEffectListview.WindowEffectType.Normal || dccData.platformName() === "wayland"
            weight: 1
            pageType: DccObject.Item
            page: ColumnLayout {
                anchors.fill: parent
                Label {
                    id: speedText
                    Layout.topMargin: 10
                    text: dccObj.displayName
                    Layout.leftMargin: 14
                }

                Flow {
                    id: listview
                    Layout.fillWidth: true
                    Layout.bottomMargin: 10
                    Layout.leftMargin: 10
                    property var tips: [qsTr("None"), qsTr("Small"), qsTr("Medium", "describe size of window rounded corners"), qsTr("Large")]
                    property var icons: ["corner_none", "corner_small", "corner_middle", "corner_big"]
                    spacing: 8
                    Repeater {
                        model: listview.tips.length
                        ColumnLayout {
                            id: layout
                            property bool checked: dccData.model.windowRadius === 6 * index
                            width: 112
                            height: 104
                            Item {
                                Layout.preferredHeight: 78
                                Layout.fillWidth: true

                                Rectangle {
                                    anchors.fill: parent
                                    color: "transparent"
                                    border.width: 2
                                    border.color: layout.checked ? D.DTK.platformTheme.activeColor : "transparent"
                                    radius: 7
                                    Control {
                                        id: iconControl
                                        anchors.fill: parent
                                        anchors.margins: 4

                                        contentItem: D.DciIcon {
                                            palette: D.DTK.makeIconPalette(iconControl.palette)
                                            theme: iconControl.D.ColorSelector.controlTheme
                                            sourceSize: Qt.size(width, height)
                                            name: listview.icons[index]
                                        }
                                    }
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        dccData.worker.setWindowRadius(6 * index)
                                    }
                                }
                            }

                            Text {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                text: listview.tips[index]
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                color: layout.checked ? D.DTK.platformTheme.activeColor : this.palette.windowText
                            }
                        }
                    }
                }
            }
        }

        DccObject {
            name: "enableTransparentWhenMoveWindow"
            parentName: "personalization/windowEffect/windowSettingsGroup"
            displayName: qsTr("Enable transparent effects when moving windows")
            visible: dccData.model.windowEffectType < InterfaceEffectListview.WindowEffectType.Normal 
                    && dccData.platformName() !== "wayland" 
                    && dccData.model.supportEffects.indexOf("kwin4_effect_translucency") !== -1
            weight: 2
            pageType: DccObject.Editor
            page: D.Switch {
                checked: dccData.model.isMoveWindow
                onCheckedChanged: {
                    dccData.worker.setMovedWindowOpacity(checked)
                }
            }
        }

        DccObject {
            id: minimizeEffectObject
            property var supportEffects: dccData.model.supportEffects
            name: "minimizeEffect"
            parentName: "personalization/windowEffect/windowSettingsGroup"
            displayName: qsTr("Window Minimize Effect")
            visible: dccData.model.windowEffectType <= InterfaceEffectListview.WindowEffectType.Best 
                    && dccData.platformName() !== "wayland"
                    && (supportEffects.indexOf("kwin4_effect_scale") !== -1 || supportEffects.indexOf("magiclamp") !== -1)
            weight: 3
            pageType: DccObject.Editor
            page: CustomComboBox {
                flat: true
                currentIndex: dccData.model.miniEffect
                textRole: "text"
                visibleRole: "support"

                model: [
                    {text: qsTr("Scale"), support: minimizeEffectObject.supportEffects.indexOf("kwin4_effect_scale") !== -1}, 
                    {text: qsTr("Magic Lamp"), support: minimizeEffectObject.supportEffects.indexOf("magiclamp") !== -1}
                ]
                onCurrentIndexChanged: {
                    dccData.worker.setMiniEffect(currentIndex)
                }
            }
        }
    }

    DccObject {
        name: "computerSuspendsAfter"
        parentName: "personalization/windowEffect"
        displayName: qsTr("Opacity")
        weight: 600
        visible: dccData.model.windowEffectType < InterfaceEffectListview.WindowEffectType.Normal || dccData.platformName() === "wayland"
        pageType: DccObject.Item
        backgroundType: DccObject.Normal
        page: ColumnLayout {
            Layout.fillHeight: true
            Label {
                Layout.topMargin: 10
                font: D.DTK.fontManager.t6
                text: dccObj.displayName
                Layout.leftMargin: 14
            }

            D.TipsSlider {
                Layout.alignment: Qt.AlignCenter
                Layout.margins: 10
                Layout.fillWidth: true
                tickDirection: D.TipsSlider.TickDirection.Back
                slider.handleType: Slider.HandleType.ArrowBottom
                slider.from: 25
                slider.to: 100
                slider.value: 25
                slider.live: true
                slider.stepSize: 1
                slider.snapMode: Slider.SnapAlways
                ticks: [
                    D.SliderTipItem {
                        text: qsTr("Low")
                    },
                    D.SliderTipItem {
                        text: ""
                    },
                    D.SliderTipItem {
                        text: qsTr("High")
                    }
                ]
                slider.onValueChanged: {
                    dccData.worker.setOpacity(slider.value)
                }

                Component.onCompleted: {
                    slider.value = dccData.model.opacity * 100
                }
            }
        }
    }

    DccObject {
        name: "titleBarHeight"
        parentName: "personalization/windowEffect"
        displayName: qsTr("Title Bar Height")
        description: qsTr("Only suitable for application window title bars drawn by the window manager.")
        weight: 700
        backgroundType: DccObject.Normal
        pageType: DccObject.Editor
        page: D.ComboBox {
            flat: true
            currentIndex: indexOfValue(dccData.model.titleBarHeight)
            model: [
                {
                    text: qsTr("Extremely small"),
                    value: 24
                },
                {
                    text: qsTr("Small"),
                    value: 32
                },
                {
                    text: qsTr("Medium", "describe height of window title bar"),
                    value: 40
                },
                {
                    text: qsTr("Large"),
                    value: 50
                }
            ]

            textRole: "text"
            valueRole: "value"

            onCurrentIndexChanged: {
                var selectedValue = model[currentIndex][valueRole]
                dccData.worker.setDiabledCompactToTitleHeight()
                dccData.worker.setTitleBarHeight(selectedValue)
            }

            function indexOfValue(value) {
                for (var i = 0; i < model.length; i++) {
                    if (model[i][valueRole] === value) {
                        return i
                    }
                }
                return -1
            }
        }
    }

    DccObject {
        id: scrollBarObject
        name: "scrollBar"
        property bool hasDBusProperty: false
        parentName: "personalization/windowEffect"
        displayName: qsTr("Scroll Bars")
        visible: dccData.model.scrollBarPolicyConfig !== "Hidden"
        weight: 800
        backgroundType: DccObject.Normal
        pageType: DccObject.Editor
        page: D.ComboBox {
            flat: true
            enabled: dccData.model.scrollBarPolicyConfig !== "Disabled"
            model: [qsTr("Show on scrolling"), qsTr("Keep shown")]
            currentIndex: {
                let policy = dccData.model.scrollBarPolicy
                if (policy === Qt.ScrollBarAsNeeded) {
                    return 0
                } else {
                    return 1
                }
            }

            onCurrentIndexChanged: {
                if (currentIndex === 0) {
                    dccData.worker.setScrollBarPolicy(Qt.ScrollBarAsNeeded)
                } else if (currentIndex === 1) {
                    dccData.worker.setScrollBarPolicy(Qt.ScrollBarAlwaysOn)
                }
            }
        }
    }

    DccObject {
        name: "compact"
        parentName: "personalization/windowEffect"
        displayName: qsTr("Compact Display")
        description: qsTr("If enabled, more content is displayed in the window.")
        visible: false && dccData.model.compactDisplayConfig !== "Hidden"
        weight: 900
        backgroundType: DccObject.Normal
        pageType: DccObject.Editor
        page: D.Switch {
            enabled: dccData.model.compactDisplayConfig !== "Disabled"
            checked: dccData.model.compactDisplay
            onCheckedChanged: {
                dccData.worker.setCompactDisplay(checked)
            }
        }
    }
}
