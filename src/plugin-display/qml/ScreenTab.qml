// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.deepin.dtk as D
import org.deepin.dcc

Item {
    id: root
    property var screen
    property alias model: repeater.model
    signal screenClicked(var screen)

    implicitHeight: 30
    RowLayout {
        Repeater {
            id: repeater
            delegate: Rectangle {
                property bool isSelect: model.modelData === screen
                property alias hovered: mouseArea.containsMouse
                implicitWidth: nameLabel.implicitWidth + 20
                implicitHeight: 30
                radius: 8
                color: isSelect ? (D.DTK.themeType == D.ApplicationHelper.LightType ? Qt.rgba(0, 0, 0, 0.1) : Qt.rgba(1, 1, 1, 0.1)) : "transparent"
                Label {
                    id: nameLabel
                    anchors.centerIn: parent
                    text: model.modelData.name
                    color: hovered ? this.palette.link : (isSelect ? this.palette.highlight : this.palette.text)
                }
                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        if (!isSelect) {
                            screenClicked(model.modelData)
                        }
                    }
                }
            }
        }
    }
}
