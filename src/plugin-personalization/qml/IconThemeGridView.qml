// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15
import QtQuick.Dialogs

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

ColumnLayout {
    id: root
    property alias model: repeater.model
    readonly property int itemWidth: 245
    readonly property int itemHeight: 97
    readonly property int itemSpacing: 10
    
    signal requsetSetTheme(string id)
    Item {
        Layout.fillWidth: true
        Layout.preferredHeight: flow.height
        Flow {
            id: flow
            property int lineCount: Math.floor((parent.width + root.itemSpacing) / (root.itemWidth + root.itemSpacing))
            width: lineCount * (root.itemWidth + root.itemSpacing) - root.itemSpacing
            spacing: 10
            anchors.horizontalCenter: parent.horizontalCenter
            Repeater {
                id: repeater
                Rectangle {
                    id: rect
                    width: root.itemWidth
                    height: root.itemHeight
                    radius: 8
                    color: this.palette.base
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 6
                        RowLayout {
                            Layout.fillWidth: true
                            Text {
                                text: model.name
                                color: this.palette.windowText
                            }

                            Item {
                                Layout.fillWidth: true
                            }

                            DccCheckIcon {
                                visible: model.checked
                            }
                        }

                        Rectangle {
                            Layout.preferredHeight: 2
                            Layout.fillWidth: true
                            color: this.palette.window
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: "transparent"
                            Image {
                                anchors.fill: parent
                                fillMode: Image.PreserveAspectFit
                                mipmap: true
                                source: model.pic
                                asynchronous: true
                            }
                        }
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (!model.checked) {
                                root.requsetSetTheme(model.id);
                            }
                        }
                    }
                }
            }
        }
    }
}
