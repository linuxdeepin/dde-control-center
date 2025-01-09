// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0
import org.deepin.dcc 1.0

Rectangle {
    id: root
    property alias model: repeater.model
    property bool backgroundVisible: true
    property bool showPlayBtn: false
    signal clicked(int index, bool checked)
    signal playbtnClicked(int index)

    color: "transparent"
    implicitHeight: layoutView.height
    Layout.fillWidth: true

    anchors.left: parent.left
    ColumnLayout {
        id: layoutView
        width: parent.width
        clip: true
        spacing: 0
        Repeater {
            id: repeater
            delegate: ItemDelegate {
                id: itemCtl
                Layout.fillWidth: true
                leftPadding: 16
                rightPadding: 10
                implicitHeight: 40
                cascadeSelected: true
                backgroundVisible: root.backgroundVisible
                contentFlow: true
                text: model.name
                hoverEnabled: true
                corners: getCornersForBackground(index, repeater.count)
                content: RowLayout {
                    Layout.alignment: Qt.AlignVCenter
                    spacing: 0
                    DciIcon {
                        Layout.alignment: Qt.AlignLeft
                        visible: showPlayBtn && model.aniIconPath.length !== 0
                        name: model.aniIconPath
                    }

                    RowLayout {
                        spacing: 0
                        Layout.alignment: Qt.AlignRight
                        IconButton {
                            Layout.alignment: Qt.AlignLeft
                            icon.name: "play_back"
                            flat: true
                            visible: showPlayBtn && itemCtl.hovered
                            implicitHeight: 20
                            implicitWidth: 20
                            icon.width: 16
                            icon.height: 16
                            onClicked: {
                                console.log("play_back has clicked ")
                                root.playbtnClicked(index)
                            }
                        }

                        DccCheckIcon {
                            checked: model.isEnabled
                            size: 16
                            onClicked: {
                                root.clicked(index, !model.isEnabled)
                            }
                        }
                    }
                }
                background: DccItemBackground {
                    separatorVisible: true
                }

                onClicked: {
                    root.clicked(index, !model.isChecked)
                }
            }
        }
    }
}
