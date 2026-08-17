// SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import org.deepin.dcc
import org.deepin.dtk as D

DccObject {
    DccObject {
        name: "camera"
        parentName: "privacy"
        displayName: qsTr("Camera")
        description: qsTr("Choose whether the application has access to the camera")
        icon: "security_camera"
        weight: 10
        Camera {}
    }

    DccObject {
        name: "filefolder"
        parentName: "privacy"
        displayName: qsTr("Files and Folders")
        description: qsTr("Choose whether the application has access to files and folders")
        icon: "security_folder"
        weight: 100
        FileAndFolder {}
    }
}
