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

    handle: PickerHandle {
        x: control.leftPadding + control.lightness * control.availableWidth - width / 2
        y: control.topPadding + (1.0 - control.saturation) * control.availableHeight - height / 2
        picker: control
        handleColor: control.color
        z: 1
    }
}
