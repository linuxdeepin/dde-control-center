// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15
import QtQuick.Dialogs
import Qt5Compat.GraphicalEffects

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dtk.private 1.0 as P

DccObject {
    DccTitleObject {
        name: "screenSaverTitle"
        weight: 10
        parentName: "personalization/screenSaver"
        displayName: qsTr("Screensaver")
    }
    DccObject {
        name: "screenSaverStatusGroup"
        parentName: "personalization/screenSaver"
        weight: 100
        pageType: DccObject.Item
        page: DccRowView { }
    }

    DccObject {
        name: "screenSaverDisplayArea"
        parentName: "personalization/screenSaver/screenSaverStatusGroup"
        weight: 200
        pageType: DccObject.Item
        page: RowLayout {
            Item {
                implicitWidth: 197
                implicitHeight: 110

                AnimatedImage {
                    id: image
                    anchors.fill: parent
                    source: dccData.model.currentScreenSaverPicMode === "" ? dccData.model.screenSaverModel.getPicPathByUrl(dccData.model.currentScreenSaver) : dccData.model.picScreenSaverModel.getPicPathByUrl(dccData.model.currentScreenSaverPicMode)
                    sourceSize: Qt.size(width, height)
                    playing: true
                    mipmap: true
                    visible: false
                    fillMode: Image.PreserveAspectCrop
                    asynchronous: true
                    onSourceChanged: {
                        playing = false
                        playing = true
                    }
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
                    border.color: Qt.rgba(0, 0, 0, 0.1)
                    border.width: 1
                }

                D.Button {
                    id: previewBtn
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 15
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 68
                    height: 24
                    visible: hoverHandler.hovered
                    text: qsTr("preview")
                    background: P.ButtonPanel {
                        implicitWidth: DS.Style.button.width
                        implicitHeight: DS.Style.button.height
                        button: previewBtn
                        opacity: 0.6
                    }
                    onClicked: {
                        dccData.worker.startScreenSaverPreview()
                    }
                }

                HoverHandler {
                    id: hoverHandler
                }
            }
        }
    }
    DccObject {
        name: "screenSaverSetGroup"
        parentName: "personalization/screenSaver/screenSaverStatusGroup"
        weight: 300
        pageType: DccObject.Item
        page: DccGroupView { isGroup: false }

        DccObject {
            name: "screenSaverSetItemGroup"
            parentName: "personalization/screenSaver/screenSaverStatusGroup/screenSaverSetGroup"
            weight: 10
            pageType: DccObject.Item
            page: DccGroupView { }

            DccObject {
                name: "personalizedScreensaver"
                parentName: "personalization/screenSaver/screenSaverStatusGroup/screenSaverSetGroup/screenSaverSetItemGroup"
                displayName: qsTr("Personalized screensaver")
                weight: 10
                pageType: DccObject.Editor
                enabled: dccData.model.screenSaverModel.getConfigAbleByUrl(dccData.model.currentScreenSaver) || dccData.model.currentScreenSaver === "deepin-custom-screensaver"
                page: D.Button {
                    implicitWidth: fm.advanceWidth(text) + leftPadding + rightPadding
                    implicitHeight: 24
                    FontMetrics {
                        id: fm
                    }
                    text: qsTr("setting")
                    onClicked: {
                        dccData.worker.requestScreenSaverConfig(dccData.model.currentScreenSaver)
                    }
                }
            }
            DccObject {
                name: "idleTime"
                parentName: "personalization/screenSaver/screenSaverStatusGroup/screenSaverSetGroup/screenSaverSetItemGroup"
                displayName: qsTr("idle time")
                weight: 100
                pageType: DccObject.Editor
                page: CustomComboBox {
                    flat: true
                    textRole: "text"
                    currentIndex: {
                        let value = dccData.model.onBattery ? dccData.model.batteryScreenSaverIdleTime : dccData.model.linePowerScreenSaverIdleTime
                        return indexByValue(value)
                    }
                    model: ListModel {
                        ListElement { text: qsTr("1 minute"); value: 60 }
                        ListElement { text: qsTr("5 minute"); value: 300 }
                        ListElement { text: qsTr("10 minute"); value: 600 }
                        ListElement { text: qsTr("15 minute"); value: 900 }
                        ListElement { text: qsTr("30 minute"); value: 1800 }
                        ListElement { text: qsTr("1 hour"); value: 3600 }
                        ListElement { text: qsTr("never"); value: 0 }
                    }
                    onCurrentIndexChanged: {
                        let value = dccData.model.onBattery ? dccData.model.batteryScreenSaverIdleTime : dccData.model.linePowerScreenSaverIdleTime
                        if (value !== model.get(currentIndex).value) {
                            dccData.worker.setScreenSaverIdleTime(model.get(currentIndex).value)
                        }
                    }
                }
            }
            DccObject {
                name: "passwordRequired"
                parentName: "personalization/screenSaver/screenSaverStatusGroup/screenSaverSetGroup/screenSaverSetItemGroup"
                displayName: qsTr("Password required for recovery")
                weight: 200
                pageType: DccObject.Editor
                page: D.Switch {
                    checked: dccData.model.lockScreenAtAwake
                    onCheckedChanged: {
                        if (checked != dccData.model.lockScreenAtAwake) {
                            dccData.worker.setLockScreenAtAwake(checked)
                        }
                    }
                }
            }
        }
    }

    DccObject {
        name: "screenSaverPicobj"
        parentName: "personalization/screenSaver"
        displayName: qsTr("Picture slideshow screensaver")
        weight: 400
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: WallpaperSelectView {
            model: dccData.model.picScreenSaverModel
            currentItem: dccData.model.currentScreenSaverPicMode
            enableContextMenu: false
            onWallpaperSelected: (url, isDark, isLock) => {
                                    // 防止调用两次
                                    if (isLock) {
                                        dccData.worker.setCurrentScreenSaverPicMode(url)
                                        dccData.worker.setScreenSaver("deepin-custom-screensaver")
                                    }
                                 }
        }
    }

    DccObject {
        name: "screenSaverListObj"
        parentName: "personalization/screenSaver"
        displayName: qsTr("System screensaver")
        weight: 500
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: WallpaperSelectView {
            model: dccData.model.screenSaverModel
            currentItem: dccData.model.currentScreenSaver
            enableContextMenu: false
            onWallpaperSelected: (url, isDark, isLock) => {
                                    // 防止调用两次
                                    if (isLock) {
                                        dccData.worker.setScreenSaver(url)
                                        if (url === "deepin-custom-screensaver") {
                                            dccData.worker.setCurrentScreenSaverPicMode("default")
                                        } else {
                                            dccData.worker.setCurrentScreenSaverPicMode("")
                                        }
                                    }
                                 }
        }
    }
}
