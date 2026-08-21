// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

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
