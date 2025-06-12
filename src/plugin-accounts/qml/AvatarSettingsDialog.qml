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
import org.deepin.dtk.style 1.0 as DS
import Qt.labs.platform 1.1

D.DialogWindow {
    id: dialog
    property string userId: dccData.currentUserId()
    property string currentAvatar: dccData.avatar()
    width: 630
    minimumWidth: width
    minimumHeight: height
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight
    icon: "preferences-system"
    modality: Qt.WindowModal
    D.DWindow.enableSystemMove: !cropper.dragActived
    color: "transparent"

    signal accepted();

    Loader {
        id: fileDlgLoader
        active: false
        sourceComponent: FileDialog {
            id: fileDialog
            nameFilters: [ qsTr("Images") + "(*.png *.bmp *.jpg *.jpeg)" ]
            title: "Please choose an image"
            folder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)
            onAccepted: {
                let selectedFile = fileDialog.selectedFile || fileDialog.file || fileDialog.fileUrl
                if (selectedFile) {
                    dialog.currentAvatar = selectedFile.toString()
                    // 23 dcc 选中自定义就会出发设置图标
                    dialog.accepted();
                }
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

    header: D.DialogTitleBar {
        // titlebar do not enbale blur again
        // enableInWindowBlendBlur: true
        icon.name: dialog.icon
    }

    RowLayout {
        width: dialog.width
        Rectangle {
            id: leftBar
            color: "transparent"
            Layout.preferredWidth: 150
            Layout.preferredHeight: dialog.height - 100
            Layout.alignment: Qt.AlignTop

            Rectangle {
                x: -DS.Style.dialogWindow.contentHMargin
                y: -DS.Style.dialogWindow.titleBarHeight
                width: leftBar.width - x
                height: dialog.height + 10
                color: "transparent"
                D.StyledBehindWindowBlur {
                    anchors.fill: parent
                }
            }
            Rectangle {
                x: leftView.width
                y: -DS.Style.dialogWindow.titleBarHeight
                width: scrollView.width + DS.Style.dialogWindow.contentHMargin * 2
                height: dialog.height
                color: palette.window
            }

            ListModel {
                id: listModel
                ListElement {
                    name: qsTr("Human")
                    filter: "icons/human"
                    icon: "dcc_user_human"
                    sections: "dimensional/dimensional_v2/flat"
                    checked: true
                }
                ListElement {
                    name: qsTr("Animal")
                    filter: "icons/animal"
                    icon: "dcc_user_animal"
                    sections: ""
                }
                ListElement {
                    name: qsTr("Scenery")
                    filter: "icons/scenery"
                    icon: "dcc_user_scenery"
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
                    id: itemDelegate
                    text: model.name
                    hoverEnabled: true
                    icon {
                        name: model.icon
                        width: 16
                        height: 16
                    }
                    implicitHeight: 30
                    implicitWidth: leftView.width - 10
                    checked: model.checked
                    background: Rectangle {
                        color: {
                            if (itemDelegate.checked) {
                                return parent.palette.highlight
                            } else if (itemDelegate.hovered) {
                                if (D.DTK.themeType == D.ApplicationHelper.LightType)
                                    return Qt.rgba(0, 0, 0, 0.1)
                                else
                                    return Qt.rgba(1, 1, 1, 0.1)
                            } else {
                                return "transparent"
                            }
                        }
                        radius: DS.Style.control.radius
                    }
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
                visible: needShow()
                onIconDropped: function (url){
                    dialog.currentAvatar = url
                    // 23 dcc 选中自定义就会出发设置图标
                    dialog.accepted();
                }
                onRequireFileDialog: {
                    fileDlgLoader.active = true
                }

                function needShow() {
                    if (!scrollView.isCustom)
                        return false

                    let icons = dccData.avatars(dialog.userId, scrollView.filter, "")
                    return icons.length < 1
                }
            }

            ScrollView {
                id: scrollView
                property list<string> sections: []
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
                            visible: view.count > 0
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

                                        customEmptyAvatar.visible = customEmptyAvatar.needShow()
                                    }
                                }
                            }

                            headerText: {
                                if (modelData === "dimensional")
                                    return qsTr("Cartoon style")
                                else if (modelData === "dimensional_v2")
                                    return qsTr("Dimensional style")
                                else if (modelData === "flat")
                                    return qsTr("Flat style")
                                else
                                    return ""
                            }

                            model: dccData.avatars(dialog.userId, scrollView.filter, modelData)
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
                Layout.leftMargin: 30
                Layout.rightMargin: 60

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
