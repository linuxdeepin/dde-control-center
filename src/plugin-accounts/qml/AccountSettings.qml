// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

DccObject {
    id: settings
    property string userId
    property string papaName
    property bool autoLoginChecked: true
    property bool nopasswdLoginChecked: true
    property bool isDeleteUser: false

    Component.onCompleted: {
        settings.autoLoginChecked = dccData.autoLogin(settings.userId)
        settings.nopasswdLoginChecked = dccData.nopasswdLogin(settings.userId)
    }

    Component.onDestruction: {
        if (isDeleteUser) {
            DccApp.showPage("accounts")
        }
    }

    // 账户头像
    DccObject {
        id: userAvatars
        name: "userAvatars"
        parentName: settings.papaName
        weight: 10
        pageType: DccObject.Item
        page: Rectangle {
            id: item
            color: "transparent"
            implicitHeight: 100

            Control {
                id: control
                implicitWidth: 100
                implicitHeight: 100
                antialiasing: true
                hoverEnabled: true
                MouseArea {
                    anchors.fill: parent
                    Loader {
                        id: dilogLoader
                        active: false
                        sourceComponent: AvatarSettingsDialog {
                            id: dialog
                            userId: settings.userId
                            currentAvatar: dccData.avatar(settings.userId)
                            onAccepted: {
                                if (currentAvatar.length > 0)
                                    dccData.setAvatar(settings.userId, currentAvatar)
                            }

                            onClosing: function (close) {
                                dilogLoader.active = false
                            }
                        }
                        onLoaded: function () {
                            dilogLoader.item.show()
                        }
                    }

                    Connections {
                        target: dccData
                        function onAvatarChanged(userId, avatar) {
                            if (userId === settings.userId) {
                                image.source = dccData.avatar(userId)
                            }
                        }
                        function onAutoLoginChanged(userId, enable) {
                            if (userId === settings.userId) {
                                settings.autoLoginChecked = enable
                            }
                        }
                        function onNopasswdLoginChanged(userId, enable) {
                            if (userId === settings.userId) {
                                settings.nopasswdLoginChecked = enable
                            }
                        }
                    }

                    onClicked: {
                        dilogLoader.active = true
                    }
                }

                // crashed https://github.com/linuxdeepin/dtkdeclarative/pull/385
                // ToolTip.text: qsTr("Clicked to changed avatar")
                // ToolTip.visible: control.hovered

                Image {
                    id: image
                    source: dccData.avatar(settings.userId)
                    asynchronous: true
                    anchors.fill: parent
                    width: 100
                    height: 100
                    fillMode: Image.PreserveAspectCrop
                    clip: true
                }

                Rectangle {
                    width: 100
                    height: 30
                    color: Qt.rgba(0, 0, 0, 0.5)
                    visible: control.hovered
                    anchors {
                        bottom: image.bottom
                        horizontalCenter: image.horizontalCenter
                    }
                }

                Text {
                    id: editText
                    text: qsTr("edit")
                    color: "white"
                    anchors {
                        horizontalCenter: control.horizontalCenter
                        bottom: control.bottom
                        bottomMargin: 2
                    }
                }

                // fake round image, just set shadowColor same with window color
                D.BoxShadow {
                    id: boxShadow
                    hollow: true
                    anchors.fill: image
                    shadowBlur: 10
                    spread: 20
                    shadowColor: control.palette.window
                    cornerRadius: control.width / 2
                }
            }

            ColumnLayout {
                id: columLayout
                anchors.left: control.right
                anchors.leftMargin: 10
                anchors.verticalCenter: item.verticalCenter
                Text {
                    text: dccData.userName(settings.userId)
                    font.pointSize: 16
                    font.bold: true
                    color: palette.text // not update ?
                }
                Text {
                    id: userTypeName
                    text: dccData.userTypeName(settings.userId)
                    color: palette.text

                    Connections {
                        target: dccData
                        function onUserTypeChanged(userId, type) {
                            if (userId === settings.userId) {
                                userTypeName.text = dccData.userTypeName(settings.userId)
                            }
                        }
                    }
                }
            }
            RowLayout {
                anchors {
                    left: columLayout.right
                    right: parent.right
                    leftMargin: 10
                    verticalCenter: item.verticalCenter
                }
                Item {
                    Layout.fillWidth: true
                }

                Button {
                    text: qsTr("Add new user")
                    Layout.alignment: Qt.AlignRight | Qt.AlignHCenter
                    Layout.rightMargin: 10
                    onClicked: {
                        cadLoader.active = true
                    }
                    Loader {
                        id: cadLoader
                        active: false
                        sourceComponent: CreateAccountDialog {
                            onClosing: function (close) {
                                cadLoader.active = false
                            }
                        }
                        onLoaded: function () {
                            cadLoader.item.show()
                        }
                    }
                }
            }
        }
    }

    // 账户信息
    DccObject {
        name: "acountInfosTitle"
        parentName: settings.papaName
        displayName: qsTr("Acount information")
        weight: 18
        pageType: DccObject.Item
        page: Label {
            leftPadding: 5
            text: dccObj.displayName
            font {
                pointSize: 13
                bold: true
            }
        }
        onParentItemChanged: item => { if (item) item.topPadding = 10 }
    }
    DccObject {
        name: settings.papaName + "acountInfos"
        parentName: settings.papaName
        displayName: qsTr("Acount Information")
        description: qsTr("Acount name, acount fullname, account type")
        weight: 20
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: settings.papaName + "acountName"
            parentName: settings.papaName + "acountInfos"
            displayName: qsTr("Acount name")
            weight: 10
            pageType: DccObject.Editor
            page: Label {
                text: dccData.userName(settings.userId)
            }
        }
        DccObject {
            name: settings.papaName + "acountFullname"
            parentName: settings.papaName + "acountInfos"
            displayName: qsTr("Acount fullname")
            weight: 20
            pageType: DccObject.Editor
            page: RowLayout {
                EditActionLabel {
                    id: fullNameEdit
                    implicitWidth: 200
                    text: dccData.fullName(settings.userId)
                    placeholderText: qsTr("Set fullname")
                    horizontalAlignment: TextInput.AlignRight
                    editBtn.visible: readOnly
                    onTextEdited: {
                        if (showAlert)
                            showAlert = false
                        // validtor can not paste invalid text..
                        var regex = /^[^:]{0,32}$/
                        if (!regex.test(text)) {
                            var filteredText = text
                            filteredText = filteredText.replace(":", "")

                            // 长度 32
                            filteredText = filteredText.slice(0, 32)
                            text = filteredText
                        }
                    }

                    onFinished: function () {
                        let alertMsg = dccData.checkFullname(text)
                        if (alertMsg.length > 0) {
                            showAlert = false
                            showAlert = true
                            alertText = alertMsg
                            readOnly = false

                            return
                        }

                        if (text.trim().length === 0) {
                            text = dccData.fullName(settings.userId)
                            return
                        }

                        dccData.setFullname(settings.userId, text)
                    }
                    Connections {
                        target: dccData
                        function onFullnameChanged(userId, fullname) {
                            if (userId === settings.userId) {
                                fullNameEdit.text = dccData.fullName(settings.userId)
                            }
                        }
                    }
                }
            }
        }
        DccObject {
            name: settings.papaName + "acountType"
            parentName: settings.papaName + "acountInfos"
            displayName: qsTr("Acount type")
            weight: 30
            pageType: DccObject.Editor
            page: ComboBox {
                flat: true
                model: dccData.userTypes()
                currentIndex: dccData.userType(settings.userId)
                enabled: dccData.isDeleteAble(settings.userId)
                onActivated: function(index) {
                    dccData.setUserType(settings.userId, index)
                }
            }
        }
    }

    // 登陆设置
    DccObject {
        name: settings.papaName + "acountSettingsTitle"
        parentName: settings.papaName
        displayName: qsTr("Login settings")
        weight: 28
        pageType: DccObject.Item
        visible: acountSettings.visible
        page: Label {
            leftPadding: 5
            text: dccObj.displayName
            font {
                pointSize: 13
                bold: true
            }
        }
        onParentItemChanged: item => { if (item) item.topPadding = 10 }
    }
    DccObject {
        id: acountSettings
        name: settings.papaName + "acountSettings"
        parentName: settings.papaName
        displayName: qsTr("Login Settings")
        description: qsTr("Auto longin, login without password")
        weight: 30
        pageType: DccObject.Item
        page: DccGroupView {}
        visible: autoLongin.visible || noPassword.visible

        DccObject {
            id: autoLongin
            name: settings.papaName + "autoLongin"
            parentName: settings.papaName + "acountSettings"
            displayName: qsTr("Auto longin")
            weight: 10
            pageType: DccObject.Editor
            visible: dccData.isAutoLoginVisable()
            enabled: dccData.currentUserId() === settings.userId
            page: Switch {
                checked: settings.autoLoginChecked
                onCheckedChanged: {
                    if (settings.autoLoginChecked != checked)
                        settings.autoLoginChecked = checked

                    dccData.setAutoLogin(settings.userId, checked)
                }
            }
        }

        DccObject {
            id: noPassword
            name: settings.papaName + "noPassword"
            parentName: settings.papaName + "acountSettings"
            displayName: qsTr("Login without password")
            weight: 20
            pageType: DccObject.Editor
            visible: dccData.isNoPassWordLoginVisable()
            enabled: dccData.currentUserId() === settings.userId
            page: Switch {
                checked: settings.nopasswdLoginChecked
                onCheckedChanged: {
                    if (settings.nopasswdLoginChecked != checked)
                        settings.nopasswdLoginChecked = checked

                    dccData.setNopasswdLogin(settings.userId, checked)
                }
            }
        }
    }

    // 登陆方式
    LoginMethod {
         name: settings.papaName + "loginMethodTitle"
         parentName: settings.papaName
         userId: settings.userId
    }

    // 动态锁
    // DccObject {
    //     name: settings.papaName + "dynamicLockTitle"
    //     parentName: settings.papaName
    //     displayName: qsTr("Dynamic lock screen")
    //     weight: 48
    //     pageType: DccObject.Item
    //     page: Label {
    //         leftPadding: 5
    //         text: dccObj.displayName
    //         font {
    //             pointSize: 13
    //             bold: true
    //         }
    //     }
    //     onParentItemChanged: item => { if (item) item.topPadding = 10 }
    // }
    // DccObject {
    //     name: settings.papaName + "dynamicLockGroups"
    //     parentName: settings.papaName
    //     displayName: qsTr("Dynamic lock screen groups")
    //     weight: 50
    //     pageType: DccObject.Item
    //     page: DccGroupView {}

    //     DccObject {
    //         name: settings.papaName + "dynamicLockItem"
    //         parentName: settings.papaName + "dynamicLockGroups"
    //         displayName: qsTr("Lock screen when device is disconnected")
    //         description: qsTr("Lock screen when the bound Bluetooth device is disconnected")
    //         weight: 10
    //         pageType: DccObject.Editor
    //         page: Switch {}
    //     }
    // }

    DccObject {
        id: bottomButtons
        name: settings.papaName + "/bottomButtons"
        parentName: settings.papaName
        weight: 0xFFFF
        pageType: DccObject.Item
        page: RowLayout {
            Button {
                Layout.alignment: Qt.AlignLeft
                text: qsTr("Delete current account")
                enabled: dccData.isDeleteAble(settings.userId)
                Loader {
                    id: cfdLoader
                    active: false
                    sourceComponent: ComfirmDeleteDialog {
                        onClosing: function (close) {
                            cfdLoader.active = false
                        }
                        onRequestDelete: function (deleteHome) {
                            settings.isDeleteUser = true
                            dccData.removeUser(settings.userId, deleteHome)
                        }
                    }
                    onLoaded: function () {
                        cfdLoader.item.show()
                    }
                }
                onClicked: {
                    cfdLoader.active = true
                }
            }

            Button {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Group setting")
                onClicked: {
                    DccApp.showPage(groupSettings)
                }
            }
        }
    }

    DccObject {
        id: groupSettings
        name: settings.papaName + "/groupSettings"
        parentName: bottomButtons.name
        displayName: qsTr("Account groups")
        weight: 70
        pageType: DccObject.Menu
        page: DccRightView {}

        DccObject {
            id: groupSettingsTitle
            property bool isEditing: false
            name: settings.papaName + "/groupSettingsTitle"
            parentName: groupSettings.name
            displayName: qsTr("Account groups")
            weight: 10
            pageType: DccObject.Item
            page: RowLayout {
                Label {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.leftMargin: 10
                    font.bold: true
                    font.pointSize: 14
                    text: dccObj.displayName
                }
                D.Button {
                    id: button
                    checkable: true
                    checked: groupSettingsTitle.isEditing
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.rightMargin: 10
                    text: groupSettingsTitle.isEditing ? qsTr("done") : qsTr("edit")
                    font.pointSize: 13
                    background: null
                    textColor: D.Palette {
                        normal {
                            common: D.DTK.makeColor(D.Color.Highlight)
                            crystal: D.DTK.makeColor(D.Color.Highlight)
                        }
                    }
                    onCheckedChanged: {
                        groupSettingsTitle.isEditing = button.checked
                    }
                }
            }
            onParentItemChanged: item => { if (item) item.topPadding = 10 }
        }

        DccObject {
            id: groupSettingsView
            name: settings.papaName + "/groupSettingsView"
            parentName: groupSettings.name
            weight: 20
            pageType: DccObject.Item
            page: DccGroupView {}

            Connections {
                target: dccData
                function onGroupsChanged(userId, groups) {
                    if (userId === settings.userId) {
                        groupsRepeater.model = dccData.groups(settings.userId)
                    }
                }
                function onGroupsUpdate() {
                    groupsRepeater.model = dccData.groups(settings.userId)
                }
                function onGroupsUpdateFailed(group) {
                    groupsRepeater.failedGroupName = group
                }
            }

            DccRepeater {
                id: groupsRepeater
                property string failedGroupName
                model: dccData.groups(settings.userId)
                delegate: DccObject {
                    name: "groupSettingsItem" + index
                    parentName: groupSettingsView.name
                    weight: 10 + 10 * index
                    pageType: DccObject.Item
                    enabled: dccData.groupEnabled(settings.userId, modelData)
                    page: Item {
                        implicitHeight: 40
                        RowLayout {
                            anchors.fill: parent
                            EditActionLabel {
                                id: edit
                                property bool editAble: dccData.groupEditAble(settings.userId, modelData)
                                property string failedText: groupsRepeater.failedGroupName
                                onFailedTextChanged: {
                                    if (failedText === text)
                                        text = modelData
                                }
                                text: modelData
                                validator: RegularExpressionValidator {
                                    // 仅使用字母、数字、下划线或短横线，并且以字母开头
                                    regularExpression: /[a-zA-Z][a-zA-Z0-9-_]{0,31}$/
                                }
                                implicitHeight: 40
                                implicitWidth: 200
                                placeholderText: qsTr("Group name")
                                horizontalAlignment: TextInput.AlignLeft | Qt.AlignVCenter
                                editBtn.visible: readOnly && editAble && !groupSettingsTitle.isEditing

                                onFinished: function () {
                                    if (text.length < 1) {
                                        text = modelData
                                        return
                                    }

                                    dccData.modifyGroup(modelData, text)
                                }
                            }

                            D.ActionButton {
                                id: editButton
                                focusPolicy: Qt.NoFocus
                                icon.width: 18
                                icon.height: 18
                                visible: groupSettingsTitle.isEditing ? edit.editAble : true
                                icon.name: {
                                    if (groupSettingsTitle.isEditing) {
                                        return "list_delete"
                                    }

                                    return editButton.checked ? "item_checked" : "item_unchecked"
                                }

                                background: null
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                // list_delete.dci icon has padding 10 (1.10p)
                                Layout.rightMargin: groupSettingsTitle.isEditing ? 0 : 10
                                // only checked from groupsChanged
                                checkable: false
                                checked: dccData.groupContains(settings.userId,
                                                               modelData)
                                onClicked: {
                                    if (groupSettingsTitle.isEditing) {
                                        // delete group
                                        dccData.deleteGroup(modelData)
                                        groupSettingsTitle.isEditing = false
                                        return
                                    }

                                    dccData.setGroup(settings.userId,modelData, !editButton.checked)
                                }
                            }
                        }
                    }
                }
            }
            DccObject {
                id: groupAddEdit
                name: "groupAddEdit"
                parentName: groupSettingsView.name
                weight: 0xFFFF
                pageType: DccObject.Item
                visible: groupAddBtn.needShowEdit && !groupSettingsTitle.isEditing
                page: Item {
                    implicitHeight: 40
                    RowLayout {
                        anchors.fill: parent
                        EditActionLabel {
                            id: editLabel
                            validator: RegularExpressionValidator {
                                // 仅使用字母、数字、下划线或短横线，并且以字母开头
                                regularExpression: /[a-zA-Z][a-zA-Z0-9-_]{0,31}$/
                            }
                            implicitHeight: 40
                            implicitWidth: 300
                            placeholderText: qsTr("Group name")
                            horizontalAlignment: TextInput.AlignLeft | Qt.AlignVCenter
                            editBtn.visible: false
                            readOnly: false
                            onFinished: function () {
                                if (text.length < 1) {
                                    return
                                }

                                dccData.createGroup(text)

                                editLabel.readOnly = false
                                groupAddBtn.needShowEdit = false
                                editLabel.text = ""
                            }
                        }
                    }
                }
            }

            DccObject {
                id: groupAddBtn
                property bool needShowEdit: false
                name: settings.papaName + "/groupAddButton"
                parentName: groupSettings.name
                weight: 0xFFFF
                pageType: DccObject.Item
                enabled: !groupSettingsTitle.isEditing
                page: RowLayout {

                    Button {
                        Layout.alignment: Qt.AlignRight
                        text: qsTr("Add group")
                        onClicked: {
                            groupAddBtn.needShowEdit = true
                        }
                    }
                }
            }
        }
    }
}
