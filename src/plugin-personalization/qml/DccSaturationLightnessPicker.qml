// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Dialogs
import QtQuick.Dialogs.quickimpl

import org.deepin.dtk as D

SaturationLightnessPickerImpl {
    id: control
    clip: true

    implicitWidth: Math.max(background ? background.implicitWidth : 0, contentItem.implicitWidth)
    implicitHeight: Math.max(background ? background.implicitHeight : 0, contentItem.implicitHeight)

    contentItem: Item {

        ShaderEffect {
            id: effect
            scale: contentItem.width / width
            anchors.fill: parent
            property alias hue: control.hue
            fragmentShader: "qrc:/qt-project.org/imports/QtQuick/Dialogs/quickimpl/shaders/SaturationLightness.frag.qsb"
        }

        D.ItemViewport {
            radius: 8
            sourceItem: effect
            hideSource: true
            anchors.fill: parent
            fixed: true
            antialiasing: true
        }
    }

    handle: Rectangle {
        implicitWidth: 12
        implicitHeight: 12
        radius: 6
        color: "transparent"
        border.color: Qt.rgba(0, 0, 0, 0.2)
        border.width: 1
        x: control.leftPadding + control.lightness * control.availableWidth - width / 2
        y: control.topPadding + (1.0 - control.saturation) * control.availableHeight - height / 2
        z: 1

        Rectangle {
            x: 1
            y: 1
            width: 10
            height: 10
            radius: 5
            color: control.color
            border.color: "white"
            border.width: 2
        }
    }
}
