// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc 1.0

D.DialogWindow {
    id: dialog
    width: 430
    height: 510
    minimumWidth: width
    minimumHeight: height
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight
    icon: "preferences-system"
    modality: Qt.ApplicationModal
    color: "transparent"

    header: D.DialogTitleBar {
        icon.name: dialog.icon
    }

    Rectangle {
        x: -DS.Style.dialogWindow.contentHMargin
        y: -DS.Style.dialogWindow.titleBarHeight
        width: dialog.width + DS.Style.dialogWindow.contentHMargin * 2
        height: dialog.height + DS.Style.dialogWindow.titleBarHeight
        color: "transparent"
        parent: dialog.contentItem
        z: -1
        D.StyledBehindWindowBlur {
            anchors.fill: parent
            blendColor: {
                if (valid) {
                    return DS.Style.control.selectColor(
                        dialog ? dialog.palette.window : undefined,
                        Qt.rgba(1, 1, 1, 0.8),
                        Qt.rgba(0.06, 0.06, 0.06, 0.8)
                    )
                }
                return DS.Style.control.selectColor(
                    undefined,
                    DS.Style.behindWindowBlur.lightNoBlurColor,
                    DS.Style.behindWindowBlur.darkNoBlurColor
                )
            }
        }
    }

    ColumnLayout {
        id: mainLayout
        width: dialog.width - 20
        spacing: 10

        ScrollView {
            id: scrollView
            Layout.fillWidth: true
            Layout.preferredHeight: 375
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AsNeeded
            contentWidth: scrollView.width
            clip: true

            TextEdit {
                id: contentLabel
                width: scrollView.availableWidth
                wrapMode: TextEdit.Wrap
                readOnly: true
                textFormat: TextEdit.MarkdownText
                font: D.DTK.fontManager.t8
                text: dccData.systemInfoMode().ueProgramLicenseContent
                selectByMouse: true
                color: D.DTK.themeType == D.ApplicationHelper.LightType ? Qt.rgba(0, 0, 0, 0.7) : Qt.rgba(1, 1, 1, 0.7)
                onLinkActivated: (link) => Qt.openUrlExternally(link)
                HoverHandler {
                    cursorShape: contentLabel.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
        }

        CheckBox {
            id: agreeCheckBox
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("Agree and Join User Experience Program")
            font: D.DTK.fontManager.t8
            checked: false
        }

        RowLayout {
            spacing: 10
            Layout.fillWidth: true

            Button {
                Layout.fillWidth: true
                text: qsTr("Cancel")
                font: D.DTK.fontManager.t7
                onClicked: {
                    dccData.systemInfoWork().cancelUeProgram()
                    close()
                }
            }
            D.RecommandButton {
                Layout.fillWidth: true
                text: qsTr("Confirm")
                font: D.DTK.fontManager.t7
                enabled: agreeCheckBox.checked
                onClicked: {
                    dccData.systemInfoWork().agreeUeProgram()
                    close()
                }
            }
        }
    }
}
