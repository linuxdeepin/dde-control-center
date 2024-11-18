// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15
import QtQuick.Dialogs
import Qt5Compat.GraphicalEffects

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    DccTitleObject {
        name: "wallpaperTitle"
        weight: 10
        parentName: "personalization/wallpaper"
        displayName: qsTr("壁纸")
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
                width: 270
                height: 180

                Image {
                    id: image
                    anchors.fill: parent
                    source: dccData.model.wallpaperMap[dccData.model.currentSelectScreen]
                    sourceSize: Qt.size(image.width, image.height)
                    visible: false
                    fillMode: Image.PreserveAspectCrop
                    asynchronous: true
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
                displayName: qsTr("Select Screen")
                weight: 10
                pageType: DccObject.Editor
                page: D.ComboBox {
                    flat: true
                    textRole: "name"
                    model: Qt.application.screens
                    onCurrentTextChanged: {
                        dccData.model.currentSelectScreen = currentText
                    }
                }
            }
            DccObject {
                name: "whenTheLidIsClosed"
                parentName: "personalization/wallpaper/wallpaperStatusGroup/wallpaperSetGroup/wallpaperSetItemGroup"
                displayName: qsTr("wave of the blue")
                visible: false
                weight: 100
                pageType: DccObject.Editor
                page: D.ComboBox {
                    width: 100
                    flat: true
                    model: ["full screen"]
                }
            }
            DccObject {
                name: "whenTheLidIsClosed"
                parentName: "personalization/wallpaper/wallpaperStatusGroup/wallpaperSetGroup/wallpaperSetItemGroup"
                displayName: qsTr("auto change wallpaper")
                visible: false
                weight: 200
                pageType: DccObject.Editor
                page: D.ComboBox {
                    width: 100
                    flat: true
                    model: ["none"]
                }
            }
        }

        DccObject {
            name: "wallpaperSetButtonGroup"
            parentName: "personalization/wallpaper/wallpaperStatusGroup/wallpaperSetGroup"
            weight: 300
            pageType: DccObject.Item
            page: DccGroupView {
                isGroup: false
            }

            DccObject {
                name: "wallpaperSetButton"
                parentName: "personalization/wallpaper/wallpaperStatusGroup/wallpaperSetGroup/wallpaperSetButtonGroup"
                weight: 2
                pageType: DccObject.Item
                page: RowLayout {
                    Item {
                        Layout.fillWidth: true
                    }
                    D.Button {
                        visible: false
                        text: "Add Image"
                    }
                    Item {
                        Layout.fillWidth: true
                    }

                    D.Button {
                        visible: false
                        text: "Add Folder"
                    }
                    Item {
                        Layout.fillWidth: true
                    }
                }
            }

        }
    }
    DccObject {
        name: "screenAndSuspendTitle"
        parentName: "personalization/wallpaper"
        // displayName: qsTr("你的图片")
        displayName: qsTr("系统壁纸")
        weight: 300
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: WallpaperSelectView {
            model: dccData.model.wallpaperModel
            currentItem: dccData.model.wallpaperMap[dccData.model.currentSelectScreen]
            onWallpaperSelected: (url, isDark, isLock) => {
                                     if (isLock) {
                                         dccData.worker.setLockBackForMonitor(dccData.model.currentSelectScreen, url, isDark)
                                     } else {
                                         dccData.worker.setBackgroundForMonitor(dccData.model.currentSelectScreen, url, isDark)
                                     }
                                 }
        }
    }

    DccObject {
        name: "screenAndSuspendTitle"
        parentName: "personalization/wallpaper"
        displayName: qsTr("每周推荐")
        weight: 300
        visible: false
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: WallpaperSelectView {
            model: 10
        }
    }

    DccObject {
        name: "screenAndSuspendTitle"
        parentName: "personalization/wallpaper"
        displayName: qsTr("炫彩")
        weight: 300
        visible: false
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: WallpaperSelectView {
            model: 34
        }
    }

    DccObject {
        name: "screenAndSuspendTitle"
        parentName: "personalization/wallpaper"
        displayName: qsTr("颜色")
        visible: false
        weight: 300
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: WallpaperSelectView {
            model: 20
        }
    }

    DccTitleObject {
        name: "sreenSaverTitle"
        weight: 400
        visible: false
        parentName: "personalization/wallpaper"
        displayName: qsTr("屏保")
    }
    DccObject {
        name: "sreenSaverStatusGroup"
        parentName: "personalization/wallpaper"
        weight: 500
        visible: false
        pageType: DccObject.Item
        page: DccRowView { }
    }

    DccObject {
        name: "sreenSaverDisplayArea"
        parentName: "personalization/wallpaper/sreenSaverStatusGroup"
        weight: 600
        visible: false
        pageType: DccObject.Item
        page: RowLayout {
            Item {
                width: 270
                height: 180

                Image {
                    anchors.fill: parent
                    id: screenSaverimage
                    source: "file:///usr/share/wallpapers/deepin/cristina-gottardi-wndpWTiDuT0-unsplash.jpg"
                    visible: false
                    fillMode: Image.PreserveAspectCrop
                }

                OpacityMask {
                    anchors.fill: parent
                    source: screenSaverimage
                    maskSource: Rectangle {
                        implicitWidth: screenSaverimage.width
                        implicitHeight: screenSaverimage.height
                        radius: 6
                    }
                }
            }
        }
    }
    DccObject {
        name: "sreenSaverSetGroup"
        parentName: "personalization/wallpaper/sreenSaverStatusGroup"
        weight: 700
        visible: false
        pageType: DccObject.Item
        page: DccGroupView {  }

        DccObject {
            name: "whenTheLidIsClosed"
            parentName: "personalization/wallpaper/sreenSaverStatusGroup/sreenSaverSetGroup"
            displayName: qsTr("照片")
            weight: 10
            pageType: DccObject.Editor
            page: D.Button {
                text: "设置"
                onClicked: {
                    screenSaverDialog.show()
                }

                D.DialogWindow {
                    id: screenSaverDialog
                    minimumWidth: 400
                    minimumHeight: 220
                    title: "照片设置"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.topMargin: 10
                        GridLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            rows:3
                            columns: 2
                            Label {
                                text: "来源:"
                            }

                            D.TextField {
                                Layout.fillWidth: true
                            }
                            Label {
                                text: "来源:"
                            }

                            D.TextField {
                                Layout.fillWidth: true
                            }
                            Item {}
                            D.CheckBox {
                                text: "随机排列图片的顺序"
                            }
                        }

                        D.DialogButtonBox {
                            Layout.fillWidth: true
                            alignment: Qt.AlignCenter
                            standardButtons: DialogButtonBox.NoButton
                            Button {
                                text: qsTr("Cancel")
                                DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
                            }
                            Button {
                                text: qsTr("Save")
                                highlighted: true
                                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                            }
                        }
                    }
                }
            }
        }
        DccObject {
            name: "whenTheLidIsClosed"
            parentName: "personalization/wallpaper/sreenSaverStatusGroup/sreenSaverSetGroup"
            displayName: qsTr("闲置时间")
            weight: 100
            pageType: DccObject.Editor
            page: D.ComboBox {
                width: 100
                flat: true
                model: ["闲置时间"]
            }
        }
        DccObject {
            name: "whenTheLidIsClosed"
            parentName: "personalization/wallpaper/sreenSaverStatusGroup/sreenSaverSetGroup"
            displayName: qsTr("auto change wallpaper")
            weight: 200
            pageType: DccObject.Editor
            page: D.ComboBox {
                width: 100
                flat: true
                model: ["30分钟"]
            }
        }
    }
    DccObject {
        name: "screenAndSuspendTitle"
        parentName: "personalization/wallpaper"
        displayName: qsTr("炫彩")
        visible: false
        weight: 800
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: WallpaperSelectView {
            model: 9
        }
    }
}
