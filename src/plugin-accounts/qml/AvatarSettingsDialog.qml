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
    width: 640
    minimumWidth: width
    minimumHeight: height
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight
    icon: "preferences-system"
    modality: Qt.WindowModal
    D.DWindow.enableSystemMove: !cropper.dragActived
    color: "transparent"

    signal accepted();

    function cleanupTempFiles() {
        if (cropper && typeof cropper.getTempPreviewFiles === 'function') {
            const files = cropper.getTempPreviewFiles()
            if (files && files.length > 0) {
                dccData.cleanupTempPreviewFiles(files)
                cropper.clearTempPreviewFiles()
            }
        }
    }

    Component.onDestruction: {
        cleanupTempFiles()
    }

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
                    let filePath = selectedFile.toString()
                    const sourceFile = filePath.replace("file://", "")
                    const saved = dccData.saveCustomAvatar(dialog.userId, sourceFile, "")
                    if (saved && saved.length > 0) {
                        dialog.currentAvatar = saved
                        scrollView.filter = "icons/local"
                        if (customEmptyAvatar && typeof customEmptyAvatar.needShow === 'function') {
                            customEmptyAvatar.visible = customEmptyAvatar.needShow()
                        }
                        if (cropper && cropper.visible) {
                            cropper.iconSource = dialog.currentAvatar
                            cropper.imgScale = 1.0
                        }
                        if (scrollView.isCustom && customLocalRow && typeof customLocalRow.refresh === 'function') {
                            customLocalRow.refresh()
                            customLocalRow.currentAvatar = dialog.currentAvatar
                        }
                    } else {
                        dialog.currentAvatar = filePath
                    }
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
        spacing: 0
        width: dialog.width
        Rectangle {
            id: leftBar
            color: "transparent"
            Layout.preferredWidth: 180 - DS.Style.dialogWindow.contentHMargin
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

            Rectangle {
                id: leftRightSplitter
                property D.Palette bgColor: D.Palette {
                    normal: Qt.rgba(0, 0, 0, 0.05)
                    normalDark: Qt.rgba(0, 0, 0, 0.5)
                }
                x: leftView.width - 1
                y: -DS.Style.dialogWindow.titleBarHeight
                width: 1
                height: dialog.height
                color: D.ColorSelector.bgColor
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
                    font: D.DTK.fontManager.t6
                    hoverEnabled: true
                    icon {
                        name: model.icon
                        width: 16
                        height: 16
                    }
                    implicitHeight: 30
                    implicitWidth: leftView.width - 10
                    checked: model.checked
                    topPadding: 0
                    bottomPadding: 2
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

                            if (model.filter !== "icons/local") {
                                if (dialog.currentAvatar && dialog.currentAvatar.length > 0) {
                                    const cacheDir = StandardPaths.writableLocation(StandardPaths.CacheLocation)
                                    const avatarDir = cacheDir + "/avatars/"
                                    const normalized = dialog.currentAvatar.replace("file://", "")
                                    const inCache = normalized.indexOf(avatarDir) === 0
                                    if (inCache) dialog.currentAvatar = ""
                                }
                            }
                        }
                    }
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 460
            Layout.minimumWidth: 460
            Layout.maximumWidth: 460

            CustomAvatarEmpatyArea {
                id: customEmptyAvatar
                visible: needShow()
                Layout.alignment: Qt.AlignHCenter
                Layout.rightMargin: 10
                onIconDropped: function (url){
                    let filePath = url.toString()
                    const sourceFile = filePath.replace("file://", "")
                    const saved = dccData.saveCustomAvatar(dialog.userId, sourceFile, "")
                    if (saved && saved.length > 0) {
                        dialog.currentAvatar = saved
                        scrollView.filter = "icons/local"
                        if (customEmptyAvatar && typeof customEmptyAvatar.needShow === 'function') {
                            customEmptyAvatar.visible = customEmptyAvatar.needShow()
                        }
                        if (cropper && cropper.visible) {
                            cropper.iconSource = dialog.currentAvatar
                            cropper.imgScale = 1.0
                        }
                        if (scrollView.isCustom && customLocalRow && typeof customLocalRow.refresh === 'function') {
                            customLocalRow.refresh()
                            customLocalRow.currentAvatar = dialog.currentAvatar
                        }
                    } else {
                        dialog.currentAvatar = filePath
                    }
                }
                onRequireFileDialog: {
                    fileDlgLoader.active = true
                }

                function needShow() {
                    if (!scrollView.isCustom)
                        return false

                    let icons = dccData.avatars(dialog.userId, scrollView.filter, "")
                    return icons.length <= 1
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
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                padding: 0
                topPadding: 0
                bottomPadding: 0
                leftPadding: 0
                rightPadding: 0
                clip: true
                contentHeight: layout.childrenRect.height
                contentWidth: width
                ScrollBar.vertical: ScrollBar {
                    id: verticalBar
                    parent: scrollView
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    x: parent.width - width - 5
                    width: 10
                    policy: ScrollBar.AsNeeded
                    visible: true
                }

                ColumnLayout {
                    id: layout
                    Layout.fillWidth: true
                    width: scrollView.width
                    implicitWidth: scrollView.width

                    Item {
                        id: customArea
                        visible: scrollView.isCustom && !customEmptyAvatar.visible
                        width: scrollView.width
                        implicitHeight: childrenRect.height

                        onVisibleChanged: {
                            if (!visible)
                                return
                            
                            let icons = dccData.avatars(dialog.userId, "icons/local", "")
                            if (!icons || icons.length <= 1) {
                                return
                            }
                            
                            const currentAvatar = (dialog.currentAvatar || "").toString().replace("file://", "")
                            let foundCurrent = false
                            
                            if (currentAvatar.length > 0) {
                                for (let i = 1; i < icons.length; i++) {
                                    const iconPath = icons[i].toString().replace("file://", "")
                                    if (iconPath === currentAvatar) {
                                        foundCurrent = true
                                        break
                                    }
                                }
                            }
                            
                            if (!foundCurrent && icons.length > 1) {
                                Qt.callLater(function() {
                                    cropper.iconSource = icons[1]
                                })
                            }
                        }

                        CustomAvatarCropper {
                            id: cropper
                            anchors.top: parent.top
                            anchors.horizontalCenter: parent.horizontalCenter
                            iconSource: {
                                const isCustom = dialog.currentAvatar.indexOf("/local/") >= 0 || dialog.currentAvatar.indexOf("/avatars/") >= 0
                                if (isCustom) {
                                    return dialog.currentAvatar
                                }
                                // 系统头像时，返回第一个自定义头像作为预览（如果有的话）
                                let icons = dccData.avatars(dialog.userId, "icons/local", "")
                                if (icons && icons.length > 1) {
                                    return icons[1]
                                }
                                return ""
                            }
                            onCroppedImage: function(file) {
                                dialog.currentAvatar = file
                            }
                            onPreviewUpdated: function(previewFile) {
                                if (customLocalRow) {
                                    customLocalRow.previewUrl = previewFile
                                }
                            }
                            onIconSourceChanged: {
                                if (customLocalRow) {
                                    customLocalRow.previewUrl = ""
                                }
                            }
                        }

                        CustomLocalAvatarsRow {
                            id: customLocalRow
                            anchors.top: cropper.bottom
                            anchors.horizontalCenter: parent.horizontalCenter

                            userId: dialog.userId
                            currentAvatar: dialog.currentAvatar
                            onCurrentAvatarChanged: dialog.currentAvatar = currentAvatar
                            onRequireFileDialog: fileDlgLoader.active = true
                        }
                    }

                    Repeater {
                        id: repeater
                        model: scrollView.sections
                        AvatarGridView {
                            id: view
                            visible: view.count > 0 && !scrollView.isCustom
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
                                    if (icons.length > 1) {
                                        dialog.currentAvatar = icons[1];
                                    }
                                }
                            }
                        }
                    }
                }
            }

            RowLayout {
                spacing: 6
                Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                Layout.bottomMargin: 6
                Layout.topMargin: 2
                Layout.leftMargin: 51
                Layout.rightMargin: 59

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
                        if (scrollView.isCustom && cropper.visible && dialog.currentAvatar.length > 0) {
                            cropper.cropToTemp(function (tmpFile) {
                                if (tmpFile && tmpFile.length > 0) {
                                    const saved = dccData.saveCustomAvatar(dialog.userId, tmpFile, dialog.currentAvatar)
                                    if (saved && saved.length > 0) {
                                        dialog.currentAvatar = saved
                                    }
                                }
                                dialog.accepted()
                                close()
                            })
                        } else {
                            dialog.accepted()
                            close()
                        }
                    }
                }
            }
        }
    }
}
