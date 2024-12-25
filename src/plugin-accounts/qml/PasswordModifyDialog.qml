// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs
import QtQuick.Window
import QtQml.Models
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dcc 1.0

D.DialogWindow {
    id: dialog
    property string userId
    width: 520
    minimumWidth: width
    minimumHeight: height
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight
    icon: "preferences-system"
    modality: Qt.WindowModal
    title: isCurrent() ? qsTr("Modify password") : qsTr("Reset password")

    function isCurrent() {
        return dialog.userId === dccData.currentUserId()
    }

    ColumnLayout {
        width: dialog.width - 20
        Label {
            text: dialog.title
            font.bold: true
            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
        }
        Label {
            text: {
                if (dialog.isCurrent())
                    return qsTr("Password length should be at least 8 characters, and the password should contain a combination of at least 3 of the following: uppercase letters, lowercase letters, numbers, and symbols. This type of password is more secure.")
                else
                    return qsTr("Resetting the password will clear the data stored in the keyring.")
            }
            wrapMode: Text.WordWrap
            rightPadding: 10
            leftPadding: 10
            // horizontalAlignment: Text.AlignHCenter
            Layout.preferredWidth: pwdLayout.minWidth(font, text, dialog.width - 20)
            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            Layout.bottomMargin: 10
        }

        PasswordLayout {
            id: pwdLayout
            userId: dialog.userId
            Layout.leftMargin: 10
            onRequestClose: {
                // no error, close dialog
                close()
            }
        }

        RowLayout {
            spacing: 10
            Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
            Layout.bottomMargin: 10
            Layout.leftMargin: 10
            Layout.rightMargin: 20

            Button {
                Layout.fillWidth: true
                text: qsTr("Cancel")
                onClicked: {
                    close()
                }
            }
            D.RecommandButton {
                Layout.fillWidth: true
                text: qsTr("Modify password")
                onClicked: {
                    if (!pwdLayout.checkPassword())
                        return

                    dccData.setPassword(dialog.userId, pwdLayout.getPwdInfo());
                }
            }
        }
    }
}
