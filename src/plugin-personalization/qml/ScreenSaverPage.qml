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
                implicitHeight: 108

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
                    border.color: D.DTK.themeType === D.ApplicationHelper.LightType ?
                                    Qt.rgba(0, 0, 0, 0.1) : Qt.rgba(1, 1, 1, 0.1)
                    border.width: 1
                }

                D.Button {
                    id: previewBtn
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 15
                    anchors.horizontalCenter: parent.horizontalCenter
                    implicitWidth: {
                        font.pixelSize
                        return previewFm.advanceWidth(text) + leftPadding + rightPadding
                    }
                    implicitHeight: 24
                    visible: hoverHandler.hovered
                    text: qsTr("preview")
                    FontMetrics {
                        id: previewFm
                        font: previewBtn.font
                    }
                    background: P.ButtonPanel {
                        property D.Palette background1: D.Palette {
                            normal {
                                common: ("#f7f7f7")
                                crystal: Qt.rgba(0, 0, 0, 0.1)
                            }
                            normalDark {
                                common: Qt.rgba(0, 0, 0, 1)
                                crystal: Qt.rgba(0, 0, 0, 1)
                            }
                            hovered {
                                common: ("#e1e1e1")
                                crystal:  Qt.rgba(16.0 / 255, 16.0 / 255, 16.0 / 255, 0.2)
                            }
                            pressed {
                                common: ("#bcc4d0")
                                crystal: Qt.rgba(16.0 / 255, 16.0 / 255, 16.0 / 255, 0.15)
                            }
                        }

                        property D.Palette background2: D.Palette {
                            normal {
                                common: ("#f0f0f0")
                                crystal: Qt.rgba(0, 0, 0, 0.1)
                            }
                            normalDark {
                                common: Qt.rgba(0, 0, 0, 1)
                                crystal: Qt.rgba(0, 0, 0, 1)
                            }
                            hovered {
                                common: ("#d2d2d2")
                                crystal: Qt.rgba(16.0 / 255, 16.0 / 255, 16.0 / 255, 0.2)
                            }
                            pressed {
                                common: ("#cdd6e0")
                                crystal: Qt.rgba(16.0 / 255, 16.0 / 255, 16.0 / 255, 0.15)
                            }
                        }
                        color1: selectValue(background1, DS.Style.checkedButton.background, background1)
                        color2: selectValue(background2, DS.Style.checkedButton.background, background2)
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
        page: ColumnLayout {
            Layout.fillWidth: true
            spacing: 0

            D.ItemDelegate {
                Layout.fillWidth: true
                Layout.preferredHeight: 36
                backgroundVisible: false
                leftPadding: 10
                rightPadding: 8
                topPadding: 0
                bottomPadding: 0
                topInset: 0
                bottomInset: 0
                corners: D.RoundRectangle.TopLeftCorner | D.RoundRectangle.TopRightCorner
                enabled: dccData.model.screenSaverModel.getConfigAbleByUrl(dccData.model.currentScreenSaver) || dccData.model.currentScreenSaver === "deepin-custom-screensaver"
                opacity: enabled ? 1 : 0.4

                contentItem: RowLayout {
                    DccLabel {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                        text: qsTr("Personalized screensaver")
                    }
                    D.Button {
                        id: settingBtn
                        Layout.alignment: Qt.AlignVCenter
                        implicitWidth: {
                            font.pixelSize
                            return fm.advanceWidth(text) + leftPadding + rightPadding
                        }
                        implicitHeight: 24
                        enabled: parent.parent.enabled
                        FontMetrics {
                            id: fm
                            font: settingBtn.font
                        }
                        text: qsTr("setting")
                        font {
                            pixelSize: D.DTK.fontManager.t7.pixelSize
                            letterSpacing: 2
                        }
                        onClicked: {
                            dccData.worker.requestScreenSaverConfig(dccData.model.currentScreenSaver)
                        }
                    }
                }

                background: DccItemBackground {
                    separatorVisible: true
                    backgroundType: 1
                }
            }

            D.ItemDelegate {
                Layout.fillWidth: true
                Layout.preferredHeight: 36
                backgroundVisible: false
                leftPadding: 10
                rightPadding: 8
                topPadding: 0
                bottomPadding: 0
                topInset: 0
                bottomInset: 0
                corners: D.RoundRectangle.NoneCorner

                contentItem: RowLayout {
                    DccLabel {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                        text: qsTr("idle time")
                    }
                    CustomComboBox {
                        Layout.alignment: Qt.AlignVCenter
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

                background: DccItemBackground {
                    separatorVisible: true
                    backgroundType: 1
                }
            }

            D.ItemDelegate {
                Layout.fillWidth: true
                Layout.preferredHeight: 36
                backgroundVisible: false
                leftPadding: 10
                rightPadding: 8
                topPadding: 0
                bottomPadding: 0
                topInset: 0
                bottomInset: 0
                corners: D.RoundRectangle.BottomLeftCorner | D.RoundRectangle.BottomRightCorner

                contentItem: RowLayout {
                    DccLabel {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                        text: qsTr("Password required for recovery")
                    }
                    D.Switch {
                        Layout.alignment: Qt.AlignVCenter
                        checked: dccData.model.lockScreenAtAwake
                        onCheckedChanged: {
                            if (checked != dccData.model.lockScreenAtAwake) {
                                dccData.worker.setLockScreenAtAwake(checked)
                            }
                        }
                    }
                }

                background: DccItemBackground {
                    separatorVisible: false
                    backgroundType: 1
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
