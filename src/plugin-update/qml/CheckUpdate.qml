// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.0
import QtQuick.Controls 2.0
import Qt.labs.qmlmodels 1.2
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D

import org.deepin.dcc 1.0

ColumnLayout {
    width: parent.width

    Rectangle {
        id: checkRoot
        width: parent.width
        height: 438
        color: "transparent"
        clip: true
        property int processVal: 0
        visible: true
        ColumnLayout {
            width: parent.width
            spacing: 10

            anchors.centerIn: parent
            Image {
                Layout.alignment: Qt.AlignHCenter
                visible: true
                source: dccData.model().checkUpdateIcon
            }

            D.ProgressBar {
                Layout.alignment: Qt.AlignHCenter
                id: process
                from: 0
                to: 1
                value: dccData.model().checkUpdateProgress
                implicitHeight: 8
                implicitWidth: 160
                visible: dccData.model().checkUpdateState == 0
            }

            D.Label {
                Layout.alignment: Qt.AlignHCenter
                width: implicitWidth
                text: dccData.model().checkUpdateStateTips
                font.pixelSize: 12
            }

            D.Button {
                Layout.alignment: Qt.AlignHCenter
                implicitWidth: 200
                font.pixelSize: 14
                visible: dccData.model().checkUpdateState != 0
                text: dccData.model().checkUpdateState === 1 ? qsTr("Check Again") : qsTr("Check for Updates")
                onClicked: {
                    dccData.work().checkUpgrade();
                }
            }

            D.Label {
                visible: false
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Last checking time")
                font.pixelSize: 10
            }
        }

        Component.onCompleted: {
            console.log(" checkUpdate :", dccData.model().upgradable)
            dccData.work().checkUpgrade();
        }
   }

}

