// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import org.deepin.dtk
import org.deepin.dcc

Label {
    id: control
    property alias hovered: mouseArea.containsMouse

    elide: Text.ElideRight
    font: DTK.fontManager.t6
    Loader {
        active: control.width < control.implicitWidth && control.hovered
        sourceComponent: Component {
            ToolTip {
                visible: true
                text: control.text
                delay: 500
                timeout: 3000
                implicitWidth: Math.min(control.implicitWidth + 10, 400)
                Connections {
                    target: DccApp
                    function onActiveObjectChanged() {
                        close()
                    }
                }
            }
        }
    }
    // 使用Attached方式退出时会崩溃
    // ToolTip.visible: width < implicitWidth && hovered
    // ToolTip.text: text
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.NoButton
    }
}
