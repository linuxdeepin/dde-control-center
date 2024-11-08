// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15
import QtQuick.Dialogs

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

GridLayout {
    id: root
    property alias model: repeater.model
    signal requsetSetTheme(string id)

    Layout.fillHeight: true
    Layout.margins: 10
    columns: width / 250
    rowSpacing: 10
    columnSpacing: 10
    Repeater {
        id: repeater
        Item {
            Layout.preferredHeight: 110
            Layout.fillWidth: true
            Rectangle {
                anchors.fill: parent
                color: "transparent"
            }
            Rectangle {
                id: rect
                anchors.centerIn: parent
                width: 245
                height: 110
                radius: 8
                color: this.palette.base
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    RowLayout {
                        Layout.preferredHeight: 20
                        Layout.fillWidth: true
                        Text {
                            text: model.id
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
                            sourceSize: Qt.size(parent.width, parent.height)
                            source: model.pic
                            asynchronous: true
                        }
                    }
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (!model.checked) {
                        root.requsetSetTheme(model.id)
                    }
                }
            }
        }
    }
}
