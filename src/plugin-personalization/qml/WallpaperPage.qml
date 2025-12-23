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

            function getQtScreen(screenName) {
                for (var s of Qt.application.screens) {
                    if (screenName === s.name) {
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

                Rectangle {
                    anchors.fill: parent
                    radius: 6
                    color: "transparent"
                    border.color: D.DTK.themeType === D.ApplicationHelper.LightType ?
                                    Qt.rgba(0, 0, 0, 0.1) : Qt.rgba(1, 1, 1, 0.1)
                    border.width: 1
                }
            }
        }
    }
    DccObject {
        name: "wallpaperSetGroup"
        parentName: "personalization/wallpaper/wallpaperStatusGroup"
        weight: 300
        pageType: DccObject.Item
        page: DccGroupView {
            Layout.alignment: Qt.AlignTop
        }

        DccObject {
            name: "wallpaperType"
            parentName: "personalization/wallpaper/wallpaperStatusGroup/wallpaperSetGroup"
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
            canSearch: false
            pageType: DccObject.Editor
            weight: 10
            onParentItemChanged: {
                if (parentItem)
                    parentItem.implicitHeight = 36
            }
        }
        DccObject {
            name: "fillStyle"
            parentName: "personalization/wallpaper/wallpaperStatusGroup/wallpaperSetGroup"
            displayName: qsTr("fill style")
            visible: false
            weight: 100
            pageType: DccObject.Editor
            page: D.ComboBox {
                flat: true
                model: ["adapt"]
            }

            onParentItemChanged: {
                if (parentItem)
                    parentItem.implicitHeight = 36
            }
        }
        DccObject {
            name: "automaticWallpaper"
            parentName: "personalization/wallpaper/wallpaperStatusGroup/wallpaperSetGroup"
            displayName: qsTr("Automatic wallpaper change")
            weight: 200
            pageType: DccObject.Editor
            page: CustomComboBox {
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
                    ListElement { text: qsTr("1 hour"); value: "3600" }
                    ListElement { text: qsTr("login"); value: "login" }
                    ListElement { text: qsTr("wake up"); value: "wakeup" }
                }
                onCurrentIndexChanged: {
                    if (indexByValue(dccData.model.wallpaperSlideShowMap[dccData.model.currentSelectScreen]) !== currentIndex) {
                        dccData.worker.setWallpaperSlideShow(dccData.model.currentSelectScreen, model.get(currentIndex).value)
                    }
                }
            }
            onParentItemChanged: {
                if (parentItem) {
                    parentItem.implicitHeight = 36
                    parentItem.rightItemTopMargin = 0
                    parentItem.rightItemBottomMargin = 0
                }
            }
        }
    }

    DccObject {
        name: "myPictures"
        parentName: "personalization/wallpaper"
        displayName: qsTr("My pictures")
        weight: 400
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: WallpaperSelectView {
            firstItemImgSource: "wallpaper_add_bg"
            firstItemTopIconName: "wallpaper_add"
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
        name: "systemWallapers"
        parentName: "personalization/wallpaper"
        displayName: qsTr("System Wallpapers")
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
        name: "liveWallpaper"
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
        name: "solidColor"
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
