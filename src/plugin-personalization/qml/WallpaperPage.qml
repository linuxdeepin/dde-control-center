// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt5Compat.GraphicalEffects

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D
import org.deepin.dcc.personalization 1.0

DccObject {
    DccTitleObject {
        name: "wallpaperTitle"
        weight: 10
        parentName: "personalization/wallpaper"
        displayName: qsTr("wallpaper")
    }


    DccObject {
        name: "screenTab"
        parentName: "personalization/wallpaper"
        weight: 50
        pageType: DccObject.Item
        visible: dccData.model.screens.length > 1
        page: ScreenTab {
            id: screemTab
            model: dccData.model.screens
            screen: dccData.model.currentSelectScreen
            onScreenChanged: {
                if (screen !== dccData.model.currentSelectScreen) {
                    dccData.model.currentSelectScreen = screen
                    if (true) {
                        indicator.createObject(this, {
                                                "screen": getQtScreen(screemTab.screen)
                                            }).show()
                    }
                }
            }

            Component {
                id: indicator
                ScreenIndicator {}
            }

            function getQtScreen(s) {
                var screen = dccData.worker.getScreen(s)
                for (var s of Qt.application.screens) {
                    if (screen.name === s.name) {
                        return s
                    }
                }
                return null
            }
        }
    }

    DccObject {
        name: "wallpaperStatusGroup"
        parentName: "personalization/wallpaper"
        weight: 100
        pageType: DccObject.Item
        page: DccRowView { }
    }

    DccObject {
        name: "wallpaparDisplayArea"
        parentName: "personalization/wallpaper/wallpaperStatusGroup"
        weight: 200
        pageType: DccObject.Item
        page: RowLayout {
            Item {
                implicitWidth: 197
                implicitHeight: 110

                Image {
                    id: image
                    anchors.fill: parent
                    source: dccData.model.wallpaperMap[dccData.model.currentSelectScreen]
                    mipmap: true
                    visible: false
                    fillMode: Image.PreserveAspectCrop
                    asynchronous: true
                    retainWhileLoading: true
                }

                OpacityMask {
                    anchors.fill: parent
                    source: image
                    maskSource: Rectangle {
                        implicitWidth: image.width
                        implicitHeight: image.height
                        radius: 6
                    }
                }
            }
        }
    }
    DccObject {
        name: "wallpaperSetGroup"
        parentName: "personalization/wallpaper/wallpaperStatusGroup"
        weight: 300
        pageType: DccObject.Item
        page: DccGroupView { isGroup: false }

        DccObject {
            name: "wallpaperSetItemGroup"
            parentName: "personalization/wallpaper/wallpaperStatusGroup/wallpaperSetGroup"
            displayName: qsTr("Window rounded corners")
            weight: 10
            pageType: DccObject.Item
            page: DccGroupView { }

            DccObject {
                name: "whenTheLidIsClosed"
                parentName: "personalization/wallpaper/wallpaperStatusGroup/wallpaperSetGroup/wallpaperSetItemGroup"
                displayName: {
                    let cutUrl = dccData.model.wallpaperMap[dccData.model.currentSelectScreen]
                    if (dccData.model.customWallpaperModel.hasWallpaper(cutUrl)) {
                        return qsTr("My pictures")
                    } else if (dccData.model.sysWallpaperModel.hasWallpaper(cutUrl)) {
                        return qsTr("System Wallpaper")
                    } else if (dccData.model.solidWallpaperModel.hasWallpaper(cutUrl)) {
                        return qsTr("Solid color wallpaper")
                    } else {
                        return qsTr("Customizable wallpapers")
                    }
                }
                pageType: DccObject.Editor
                weight: 10
            }
            DccObject {
                name: "whenTheLidIsClosed"
                parentName: "personalization/wallpaper/wallpaperStatusGroup/wallpaperSetGroup/wallpaperSetItemGroup"
                displayName: qsTr("fill style")
                visible: false
                weight: 100
                pageType: DccObject.Editor
                page: D.ComboBox {
                    width: 100
                    flat: true
                    model: ["adapt"]
                }
            }
            DccObject {
                name: "whenTheLidIsClosed"
                parentName: "personalization/wallpaper/wallpaperStatusGroup/wallpaperSetGroup/wallpaperSetItemGroup"
                displayName: qsTr("Automatic wallpaper change")
                weight: 200
                pageType: DccObject.Editor
                page: CustomComboBox {
                    implicitWidth: 170
                    flat: true
                    textRole: "text"
                    currentIndex: indexByValue(dccData.model.wallpaperSlideShowMap[dccData.model.currentSelectScreen])
                    model: ListModel {
                        ListElement { text: qsTr("never"); value: "" }
                        ListElement { text: qsTr("30 second"); value: "30" }
                        ListElement { text: qsTr("1 minute"); value: "60" }
                        ListElement { text: qsTr("5 minute"); value: "300" }
                        ListElement { text: qsTr("10 minute"); value: "600" }
                        ListElement { text: qsTr("15 minute"); value: "900" }
                        ListElement { text: qsTr("30 minute"); value: "1800" }
                        ListElement { text: qsTr("1 hover"); value: "3600" }
                        ListElement { text: qsTr("login"); value: "login" }
                        ListElement { text: qsTr("wake up"); value: "wakeup" }
                    }
                    onCurrentIndexChanged: {
                        if (indexByValue(dccData.model.wallpaperSlideShowMap[dccData.model.currentSelectScreen]) !== currentIndex) {
                            dccData.worker.setWallpaperSlideShow(dccData.model.currentSelectScreen, model.get(currentIndex).value)
                        }
                    }
                }
            }
        }
    }

    DccObject {
        name: "screenAndSuspendTitle"
        parentName: "personalization/wallpaper"
        displayName: qsTr("My pictures")
        weight: 400
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: WallpaperSelectView {
            firstItemImgSource: "wallpaper_add"
            model: dccData.model.customWallpaperModel
            currentItem: dccData.model.wallpaperMap[dccData.model.currentSelectScreen]
            onFirstItemClicked: {
                customWallpaperFileDialog.open()
            }
            onWallpaperSelected: (url, isDark, option) => {
                                    dccData.worker.setWallpaperForMonitor(dccData.model.currentSelectScreen, url, isDark, option)
                                 }
            
            onWallpaperDeleteClicked: (url) => {
                dccData.worker.deleteWallpaper(url)
            }

            FileDialog {
                id: customWallpaperFileDialog
                title: "Choose an Image File"
                nameFilters: ["Image files (*.png *.jpg *.jpeg *.bmp *.tiff)", "All files (*)"]
                fileMode: FileDialog.OpenFile
                onSelectedFilesChanged: {
                    dccData.worker.addCustomWallpaper(customWallpaperFileDialog.selectedFile)
                }
            }
        }
    }

    DccObject {
        name: "screenAndSuspendTitle"
        parentName: "personalization/wallpaper"
        displayName: qsTr("System Wallapers")
        weight: 500
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: WallpaperSelectView {
            model: dccData.model.sysWallpaperModel
            currentItem: dccData.model.wallpaperMap[dccData.model.currentSelectScreen]
            onWallpaperSelected: (url, isDark, option) => {
                                    dccData.worker.setWallpaperForMonitor(dccData.model.currentSelectScreen, url, isDark, option)
                                 }
        }
    }

    DccObject {
        name: "screenAndSuspendTitle"
        parentName: "personalization/wallpaper"
        visible: false
        displayName: qsTr("Live Wallpaper")
        weight: 600
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: WallpaperSelectView {
            // model: dccData.model.wallpaperModel
        }
    }

    DccObject {
        name: "screenAndSuspendTitle"
        parentName: "personalization/wallpaper"
        displayName: qsTr("Solid color wallpaper")
        weight: 600
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: WallpaperSelectView {
            firstItemImgSource: "wallpaper_addcolor"
            model: dccData.model.solidWallpaperModel
            currentItem: dccData.model.wallpaperMap[dccData.model.currentSelectScreen]
            onWallpaperSelected: (url, isDark, option) => {
                                    dccData.worker.setWallpaperForMonitor(dccData.model.currentSelectScreen, url, isDark, option)
                                 }
            onFirstItemClicked: {
                colorDialog.open()
            }
            onWallpaperDeleteClicked: (url) => {
                dccData.worker.deleteWallpaper(url)
            }
            DccColorDialog {
                id: colorDialog
                anchors.centerIn: Overlay.overlay
                popupType: Popup.Item
                width: 300
                height: 300
                onAccepted: {
                    dccData.worker.addSolidWallpaper(colorDialog.color)
                }
            }
        }
    }
}
