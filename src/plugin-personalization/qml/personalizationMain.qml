// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D
import org.deepin.dcc.personalization 1.0

DccObject {
    DccObject {
        name: "themeRoot"
        parentName: "personalization"
        weight: 10
        pageType: DccObject.Item
        page: DccGroupView {}

        onActive: function (cmdParam) {
            dccData.handleCmdParam(PersonalizationData.Theme, cmdParam)
        }

        DccObject {
            name: "themeTitle"
            parentName: "personalization/themeRoot"
            displayName: qsTr("Theme")
            weight: 1
            pageType: DccObject.Item
            page: ColumnLayout {
                Layout.fillHeight: true
                Layout.margins: 10
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    Layout.topMargin: 10
                    Layout.rightMargin: 10
                    Label {
                        font: D.DTK.fontManager.t6
                        text: dccObj.displayName
                        Layout.leftMargin: 10
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    D.IconButton {
                        flat: true
                        enabled: themeSelectView.currentIndex !== 0
                        icon.name: "arrow_left"
                        icon.width: 16
                        icon.height: 16
                        implicitWidth: 16
                        implicitHeight: 16
                        onClicked: {
                            themeSelectView.decrementCurrentIndex()
                        }
                        background: null
                    }
                    D.IconButton {
                        flat: true
                        enabled: themeSelectView.currentIndex !== themeSelectView.count - 1
                        icon.name: "arrow_right"
                        icon.width: 16
                        icon.height: 16
                        implicitWidth: 16
                        implicitHeight: 16
                        onClicked: {
                            themeSelectView.incrementCurrentIndex()
                        }
                        background: null
                    }
                }
                ThemeSelectView {
                    id: themeSelectView
                    Layout.fillWidth: true
                    Layout.preferredHeight: 240
                    Layout.bottomMargin: 15
                }
            }
        }

        DccObject {
            name: "appearance"
            parentName: "personalization/themeRoot"
            displayName: qsTr("Appearance")
            description: qsTr("Select light, dark or automatic theme appearance")
            weight: 2
            pageType: DccObject.Editor
            icon: "appearance"
            page: D.ComboBox {
                flat: true
                textRole: "text"
                model: dccData.appearanceSwitchModel
                currentIndex: {
                    for (var i = 0; i < model.length; ++i) {
                        if (model[i].value === dccData.currentAppearance) {
                            return i;
                        }
                    }
                    return 0
                }

                enabled: model.length > 1
                onCurrentIndexChanged: {
                    if (dccData.currentAppearance !== model[currentIndex].value) {
                        dccData.worker.setAppearanceTheme(model[currentIndex].value)
                    }
                }
            }
        }
    }

    DccObject {
        name: "windowEffect"
        parentName: "personalization"
        displayName: qsTr("Window effect")
        description: qsTr("Interface and effects, rounded corners")
        icon: "window_effect"
        weight: 200
        WindowEffectPage {}
    }
    DccObject {
        name: "wallpaper"
        parentName: "personalization"
        displayName: qsTr("Wallpaper")
        description: qsTr("Personalize your wallpaper and screensaver")
        icon: "dcc_wallpaper"
        weight: 300
        onActive: function (cmdParam) {
            dccData.handleCmdParam(PersonalizationData.Wallpaper, cmdParam)
        }
        WallpaperPage {}
    }
    DccObject {
        name: "screenSaver"
        parentName: "personalization"
        displayName: qsTr("Screensaver")
        description: qsTr("Personalize your wallpaper and screensaver")
        icon: "screensaver"
        weight: 400
        ScreenSaverPage {}
    }
    DccObject {
        name: "colorAndIcons"
        parentName: "personalization"
        displayName: qsTr("Colors and icons")
        description: qsTr("Adjust accent color and theme icons")
        icon: "icon_cursor"
        weight: 500
        ColorAndIcons {}
    }
    DccObject {
        name: "font"
        parentName: "personalization"
        displayName: qsTr("Font and font size")
        description: qsTr("Change system font and size")
        icon: "font_size"
        weight: 600
        FontSizePage {}
    }
}
