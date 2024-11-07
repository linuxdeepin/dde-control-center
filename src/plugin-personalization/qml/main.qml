// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    DccObject {
        name: "themeGroup"
        parentName: "personalization"
        weight: 10
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "themeTitle"
            parentName: "personalization/themeGroup"
            displayName: qsTr("Theme")
            weight: 1
            pageType: DccObject.Item
            page: ColumnLayout {
                Layout.fillHeight: true
                Layout.margins: 10
                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        Layout.topMargin: 10
                        font: D.DTK.fontManager.t5
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
                        icon.name: "arrow_ordinary_left"
                        onClicked: {
                            themeSelectView.decrementCurrentIndex()
                        }
                    }
                    D.IconButton {
                        flat: true
                        enabled: themeSelectView.currentIndex !== themeSelectView.count - 1
                        icon.name: "arrow_ordinary_right"
                        onClicked: {
                            themeSelectView.incrementCurrentIndex()
                        }
                    }
                }
                ThemeSelectView {
                    id: themeSelectView
                    Layout.fillWidth: true
                    Layout.preferredHeight: 240
                }
            }
        }

        DccObject {
            name: "appearance"
            parentName: "personalization/themeGroup"
            displayName: qsTr("Appearance")
            description: qsTr("The appearance determines whether the theme displays light or dark colors, or automatically switches")
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
                    dccData.worker.setAppearanceTheme(model[currentIndex].value)
                }
            }
        }
    }

    DccObject {
        name: "windowEffect"
        parentName: "personalization"
        displayName: qsTr("Window effect")
        description: qsTr("Set interface effects and icon sizes")
        icon: "window_effect"
        weight: 200
        WindowEffectPage {}
    }
    DccObject {
        name: "wallpaper"
        parentName: "personalization"
        displayName: qsTr("Wallpaper and screensaver")
        description: qsTr("Personalize your wallpaper and screensaver")
        icon: "wallpaper"
        weight: 300
        WallpaperPage {}
    }
    DccObject {
        name: "colorAndIcons"
        parentName: "personalization"
        displayName: qsTr("Colors and icons")
        description: qsTr("Adjust preferred activity colors and theme icons")
        icon: "icon_cursor"
        weight: 400
        ColorAndIcons {}
    }
    DccObject {
        name: "font"
        parentName: "personalization"
        displayName: qsTr("Font and font size")
        description: qsTr("Modify the system font and font size")
        icon: "font_size"
        weight: 500
        FontSizePage {}
    }
}
