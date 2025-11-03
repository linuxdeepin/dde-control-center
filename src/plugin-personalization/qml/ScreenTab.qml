// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D

Item {
    id: root
    property var screen
    property alias model: repeater.model

    implicitHeight: 30
    RowLayout {
        Repeater {
            id: repeater
            delegate: Rectangle {
                property bool isSelect: modelData === screen
                property alias hovered: mouseArea.containsMouse
                implicitWidth: nameLabel.implicitWidth + 20
                implicitHeight: 30
                anchors.topMargin: 2
                radius: 8
                color: isSelect ? this.palette.midlight : "transparent"
                Rectangle {
                    color: isSelect ? (D.DTK.themeType == D.ApplicationHelper.LightType ? Qt.rgba(0, 0, 0, 0.1) : Qt.rgba(1, 1, 1, 0.1)) : "transparent"
                    anchors.fill: parent
                    anchors.topMargin: -2
                    anchors.bottomMargin: 2
                    radius: 8
                    Label {
                        id: nameLabel
                        anchors.centerIn: parent
                        text: modelData
                        color: hovered ? this.palette.link : (isSelect ? this.palette.highlight : this.palette.text)
                    }
                }
                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        if (!isSelect) {
                            screen = modelData
                        }
                    }
                }
            }
        }
    }
}
