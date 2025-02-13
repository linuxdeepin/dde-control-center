// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

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
                anchors.topMargin: 2
                radius: 8
                color: isSelect ? this.palette.midlight : "transparent"
                Rectangle {
                    color: isSelect ? this.palette.toolTipBase : "transparent"
                    anchors.fill: parent
                    anchors.topMargin: -2
                    anchors.bottomMargin: 2
                    radius: 8
                    Label {
                        id: nameLabel
                        anchors.centerIn: parent
                        text: model.modelData.name
                        color: hovered ? this.palette.link : (isSelect ? this.palette.highlight : this.palette.text)
                    }
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
