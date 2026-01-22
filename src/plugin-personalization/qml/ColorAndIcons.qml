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

DccObject {
    DccTitleObject {
        name: "accentColorTitle"
        weight: 10
        parentName: "personalization/colorAndIcons"
        displayName: qsTr("Accent Color")
    }

    DccObject {
        name: "accentColor"
        parentName: "personalization/colorAndIcons"
        weight: 100
        pageType: DccObject.Item
        backgroundType: DccObject.Normal
        page: ListView {
            id: listview
            readonly property var colors: ["#DF4187", "#EA691F", "#F3B517", "#49B125", "#00A48A", "#1F6EE7", "#402FDB", "#7724B1", "#757575", "CUSTOM"]
            readonly property var darkColors: ["#A82B62", "#CC4D03", "#D09C00", "#459F29", "#188876", "#024CCA", "#443BBA", "#6A2487", "#868686", "CUSTOM"]
            property var cutColors: dccData.currentAppearance === ".dark" ? darkColors : colors
            implicitHeight: 60
            leftMargin: 10
            clip: true
            model: cutColors.length
            orientation: ListView.Horizontal
            layoutDirection: Qt.LeftToRight
            spacing: 12

            delegate: Item {
                anchors.verticalCenter: parent.verticalCenter
                property string activeColor: dccData.model.activeColor
                property string currentColor: listview.cutColors[index]
                width: 30
                height: 30
                Rectangle {
                    anchors.fill: parent
                    border.width: 2
                    color: "transparent"
                    visible: activeColor === currentColor || (currentColor == "CUSTOM" && listview.cutColors.indexOf(activeColor) === -1)
                    border.color: currentColor == "CUSTOM" ? activeColor : currentColor
                    radius: width / 2
                }

                Rectangle {
                    id: colorImg
                    anchors.fill: parent
                    anchors.margins: 4
                    radius: width / 2
                    color: listview.cutColors[index] === "CUSTOM" ? "transparent" : listview.cutColors[index]
                    anchors.centerIn: parent

                    Canvas {
                        anchors.fill: parent
                        visible: listview.cutColors[index] === "CUSTOM"
                        onPaint: {
                            var ctx = getContext("2d");
                            ctx.clearRect(0, 0, width, height);

                            var centerX = width / 2;
                            var centerY = height / 2;
                            var radius = Math.min(width, height) / 2;

                            var gradient = ctx.createConicalGradient(centerX, centerY, 0, centerX, centerY, radius);

                            gradient.addColorStop(0.0, "red");
                            gradient.addColorStop(0.167, "yellow");
                            gradient.addColorStop(0.333, "green");
                            gradient.addColorStop(0.5, "cyan");
                            gradient.addColorStop(0.667, "blue");
                            gradient.addColorStop(0.833, "magenta");
                            gradient.addColorStop(1.0, "red");

                            ctx.fillStyle = gradient;
                            ctx.beginPath();
                            ctx.arc(centerX, centerY, radius, 0, 2 * Math.PI);
                            ctx.closePath();
                            ctx.fill();
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (listview.cutColors[index] === "CUSTOM") {
                                colorDialog.color = dccData.model.activeColor
                                colorDialog.open()
                            } else {
                                dccData.worker.setActiveColors(listview.colors[index] + "," + listview.darkColors[index])
                                dccData.worker.setActiveColor(listview.cutColors[index])
                            }
                        }
                    }
                }

                InnerShadow {
                    anchors.fill: colorImg
                    radius: 0
                    samples: 24
                    spread: 0
                    horizontalOffset: 0
                    verticalOffset: -1
                    color: Qt.rgba(0, 0, 0, 0.1)
                    source: colorImg
                }
            }
            DccColorDialog {
                id: colorDialog
                anchors.centerIn: Overlay.overlay
                popupType: Popup.Item
                width: 300
                height: 300
                onAccepted: {
                    dccData.worker.setActiveColors(colorDialog.color + "," + colorDialog.color)
                    dccData.worker.setActiveColor(colorDialog.color)
                }
            }
        }
    }

    DccTitleObject {
        name: "iconSettingsTitle"
        parentName: "personalization/colorAndIcons"
        displayName: qsTr("Icon Settings")
        weight: 200
    }

    DccObject {
        name: "iconTheme"
        parentName: "personalization/colorAndIcons"
        displayName: qsTr("Icon Theme")
        description: qsTr("Customize your theme icon")
        icon: "theme_icon"
        weight: 300
        backgroundType: DccObject.Normal
        pageType: DccObject.MenuEditor
        page: Label {
            text: dccData.model.iconModel.currentThemeName
        }

        DccObject {
            name: "iconThemeSelect"
            parentName: "personalization/colorAndIcons/iconTheme"
            weight: 1
            DccObject {
                name: "cursorThemeSelect"
                parentName: "personalization/colorAndIcons/iconTheme/iconThemeSelect"
                weight: 1
                pageType: DccObject.Item
                page: IconThemeGridView {
                    model: dccData.iconThemeViewModel
                    onRequsetSetTheme: (id)=> {
                                           dccData.worker.setIconTheme(id)
                                       }
                }
            }
        }
    }
    DccObject {
        name: "cursorTheme"
        parentName: "personalization/colorAndIcons"
        displayName: qsTr("Cursor Theme")
        description: qsTr("Customize your theme cursor")
        icon: "topic_cursor"
        weight: 400
        backgroundType: DccObject.Normal
        pageType: DccObject.MenuEditor
        page: Label {
            text: dccData.model.cursorModel.currentThemeName
        }

        DccObject {
            name: "cursorThemeSelect"
            parentName: "personalization/colorAndIcons/cursorTheme"
            weight: 1
            DccObject {
                name: "cursorThemeSelect"
                parentName: "personalization/colorAndIcons/cursorTheme/cursorThemeSelect"
                weight: 1
                pageType: DccObject.Item
                page: IconThemeGridView {
                    model: dccData.cursorThemeViewModel
                    onRequsetSetTheme: (id)=> {
                                           dccData.worker.setCursorTheme(id)
                                       }
                }
            }
        }
    }
}
