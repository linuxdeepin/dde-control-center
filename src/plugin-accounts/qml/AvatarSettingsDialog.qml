// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs
import QtQuick.Window
import QtQml.Models
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels
import org.deepin.dtk 1.0 as D

D.DialogWindow {
    id: dialog
    property string userId: dccData.currentUserId()
    property string currentAvatar: dccData.avatar()
    width: 640
    height: 500
    icon: "preferences-system"
    modality: Qt.WindowModal
    D.DWindow.enableSystemMove: !cropper.dragActived

    signal accepted();

    Loader {
        id: fileDlgLoader
        active: false
        sourceComponent: FileDialog {
            id: fileDialog
            nameFilters: [ qsTr("Images") + "(*.png *.bmp *.jpg *.jpeg)" ]
            title: "Please choose an image"
            onAccepted: {
                dialog.currentAvatar = fileDialog.selectedFile
                // 23 dcc 选中自定义就会出发设置图标
                dialog.accepted();

                fileDlgLoader.active = false
            }
            onRejected: {
                fileDlgLoader.active = false
            }
        }
        onLoaded: function () {
            fileDlgLoader.item.open()
        }
    }

    RowLayout {
        anchors.fill: parent
        Rectangle {
            color: "transparent"
            Layout.preferredWidth: 160
            Layout.preferredHeight: dialog.height - 100
            Layout.alignment: Qt.AlignTop

            ListModel {
                id: listModel
                ListElement {
                    name: qsTr("Human")
                    filter: "icons/human"
                    icon: "dcc_user_human"
                    sections: "dimensional/flat"
                    checked: true
                }
                ListElement {
                    name: qsTr("Animal")
                    filter: "icons/animal"
                    icon: "dcc_user_animal"
                    sections: ""
                }
                ListElement {
                    name: qsTr("Illustration")
                    filter: "icons/illustration"
                    icon: "dcc_user_funny"
                    sections: ""
                }
                ListElement {
                    name: qsTr("Emoji")
                    filter: "icons/emoji"
                    icon: "dcc_user_emoji"
                    sections: ""
                }
                ListElement {
                    name: qsTr("custom")
                    filter: "icons/local"
                    icon: "dcc_user_custom"
                    sections: ""
                }
            }

            D.ListView {
                id: leftView
                clip: true
                implicitHeight: 400
                implicitWidth: parent.width
                Layout.alignment: Qt.AlignTop
                model: listModel
                delegate: D.ItemDelegate {
                    text: model.name
                    icon.name: model.icon
                    implicitWidth: 160
                    checked: model.checked
                    normalBackgroundVisible: false
                    onCheckedChanged: {
                        if (checked) {
                            let sections = model.sections.split('/')
                            // reset model...
                            scrollView.sections = []
                            scrollView.sections = sections.length > 0 ? sections : [""]
                            scrollView.filter = model.filter
                            scrollView.ScrollBar.vertical.position = 0
                        }
                    }
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            CustomAvatarEmpatyArea {
                id: customEmptyAvatar
                visible: {
                    if (!scrollView.isCustom)
                        return false

                    let icons = dccData.avatars(dialog.userId, scrollView.filter, "")
                    return icons.length < 1
                }

                onIconDropped: function (url){
                    dialog.currentAvatar = url
                }
                onRequireFileDialog: {
                    fileDlgLoader.active = true
                }
            }

            ScrollView {
                id: scrollView
                property list<string> sections: ["dimensional", "flat"]
                property string filter
                property bool isCustom: filter === "icons/local"
                implicitHeight: 360
                visible: !customEmptyAvatar.visible
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop
                contentHeight: layout.childrenRect.height

                ColumnLayout {
                    id: layout
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    CustomAvatarCropper {
                        id: cropper
                        visible: scrollView.isCustom && !customEmptyAvatar.visible
                        iconSource: dialog.currentAvatar
                        onCroppedImage: function(file) {
                            dialog.currentAvatar = file
                            dialog.accepted();
                        }
                    }

                    Repeater {
                        id: repeater
                        model: scrollView.sections
                        AvatarGridView {
                            id: view
                            currentAvatar: dialog.currentAvatar
                            onCurrentAvatarChanged: {
                                dialog.currentAvatar = currentAvatar
                                if (cropper.visible) {
                                    cropper.iconSource = currentAvatar
                                    cropper.imgScale = 1.0
                                }
                            }
                            onRequireFileDialog: {
                                fileDlgLoader.active = true
                            }

                            Connections {
                                target: dccData
                                function onAvatarChanged(userId, avatar) {
                                    if (userId === dialog.userId) {
                                        dialog.currentAvatar = avatar
                                        view.currentAvatar = avatar
                                        view.model = []
                                        view.model = dccData.avatars(dialog.userId, scrollView.filter, modelData)
                                    }
                                }
                            }

                            headerText: {
                                if (modelData === "dimensional")
                                    return qsTr("Dimensional style")
                                else if (modelData === "flat")
                                    return qsTr("Flat style")
                                else
                                    return ""
                            }

                            model: dccData.avatars(dialog.userId, scrollView.filter, modelData)
                            isCustom: scrollView.isCustom
                            onIsCustomChanged: {
                                if (isCustom && !customEmptyAvatar.visible) {
                                    let icons = dccData.avatars(dialog.userId, scrollView.filter, modelData)
                                    if (icons.length > 1)
                                        dialog.currentAvatar = icons[1]; // [0] is add button
                                }
                            }
                        }
                    }
                }
            }

            RowLayout {
                spacing: 10
                Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                Layout.bottomMargin: 10
                Layout.topMargin: 20

                Button {
                    Layout.fillWidth: true
                    text: qsTr("Cancel")
                    onClicked: {
                        // reset
                        dialog.currentAvatar = ""
                        close()
                    }
                }
                D.RecommandButton {
                    Layout.fillWidth: true
                    text: qsTr("Save")
                    onClicked: {
                        dialog.accepted()
                        close()
                    }
                }
            }
        }
    }
}
