
// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0
import org.deepin.dtk.style 1.0 as DS

DccObject {
    id: settings
    property string userId
    property string papaName
    property bool autoLoginChecked: true
    property bool nopasswdLoginChecked: true

    Component.onCompleted: {
        settings.autoLoginChecked = dccData.autoLogin(settings.userId)
        settings.nopasswdLoginChecked = dccData.nopasswdLogin(settings.userId)
    }

    FontMetrics {
        id: fm
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
                        function onUserRemoved(userId) {
                            if (userId === settings.userId) {
                                DccApp.showPage("accounts")
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
                    visible: control.hovered
                    anchors {
                        bottom: image.bottom
                        horizontalCenter: image.horizontalCenter
                    }
                    // color: Qt.rgba(0, 0, 0, 0.5)
                    gradient: Gradient {
                        GradientStop {  position: 0.0; color: Qt.rgba(0, 0, 0, 0) }
                        GradientStop {  position: 1.0; color: Qt.rgba(0, 0, 0, 1) }
                    }
                }

                Text {
                    id: editText
                    text: qsTr("edit")
                    color: "white"
                    visible: control.hovered
                    anchors {
                        horizontalCenter: control.horizontalCenter
                        bottom: control.bottom
                        bottomMargin: 2
                    }
                }

                // fake round image, just set shadowColor same with window color
                BoxShadow {
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
                    implicitWidth: fm.advanceWidth(text) + 12
                    implicitHeight: 30
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
        displayName: qsTr("Account Information")
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
        displayName: qsTr("Account Information")
        description: qsTr("Account name, account fullname, account type")
        weight: 20
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: settings.papaName + "acountName"
            parentName: settings.papaName + "acountInfos"
            displayName: qsTr("Account name")
            weight: 10
            pageType: DccObject.Editor
            page: Label {
                text: dccData.userName(settings.userId)
            }
        }
        DccObject {
            name: settings.papaName + "acountFullname"
            parentName: settings.papaName + "acountInfos"
            displayName: qsTr("Account fullname")
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
            displayName: qsTr("Account type")
            weight: 30
            pageType: DccObject.Editor
            enabled: dccData.isDeleteAble(settings.userId)
            page: ComboBox {
                flat: true
                model: dccData.userTypes()
                currentIndex: dccData.userType(settings.userId)
                onActivated: function (index) {
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
        description: qsTr("Auto login, login without password")
        weight: 30
        pageType: DccObject.Item
        page: DccGroupView {}
        visible: (autoLongin.visible || noPassword.visible) && !DccApp.isTreeland()

        DccObject {
            id: autoLongin
            name: settings.papaName + "autoLongin"
            parentName: settings.papaName + "acountSettings"
            displayName: qsTr("Auto login")
            weight: 10
            pageType: DccObject.Editor
            visible: dccData.isAutoLoginVisable()
            enabled: dccData.currentUserId() === settings.userId
            page: Switch {
                checked: settings.autoLoginChecked
                onCheckedChanged: {
                    if (settings.autoLoginChecked != checked)
                        settings.autoLoginChecked = checked

                    if (checked) {
                        var userName = dccData.getOtherUserAutoLogin()
                        if (userName.length > 0) {
                            awdLoader.active = true
                            awdLoader.item.userName = userName
                            return
                        }
                    }

                    dccData.setAutoLogin(settings.userId, checked)
                }

                Loader {
                    id: awdLoader
                    active: false
                    sourceComponent: AutoLoginWarningDialog {
                        onClosing: function (close) {
                            awdLoader.active = false
                            settings.autoLoginChecked = false
                        }
                    }
                    onLoaded: function () {
                        awdLoader.item.show()
                    }
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
                id: deleteBtn
                Layout.alignment: groupSettingsBtn.visible ? Qt.AlignLeft : Qt.AlignRight
                text: qsTr("Delete current account")
                enabled: dccData.isDeleteAble(settings.userId)
                implicitWidth: fm.advanceWidth(text) + 12
                implicitHeight: 30
                Loader {
                    id: cfdLoader
                    active: false
                    sourceComponent: ComfirmDeleteDialog {
                        onClosing: function (close) {
                            cfdLoader.active = false
                        }
                        onRequestDelete: function (deleteHome) {
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
                Connections {
                    target: dccData
                    function onOnlineUserListChanged() {
                        deleteBtn.enabled = dccData.isDeleteAble(settings.userId)
                    }
                }
            }

            Button {
                id: groupSettingsBtn
                Layout.alignment: Qt.AlignRight
                text: qsTr("Group setting")
                visible: dccData.needShowGroups()
                implicitWidth: fm.advanceWidth(text) + 12
                implicitHeight: 30
                onClicked: {
                    DccApp.showPage(groupSettings)
                }
            }
        }
    }

    DccObject {
        id: groupSettings
        property bool isEditing
        name: settings.papaName + "/groupSettings"
        parentName: bottomButtons.name
        displayName: qsTr("Account groups")
        weight: 10
        pageType: DccObject.Menu
        page: ListView {
            id: groupview
            property int lrMargin: DccUtils.getMargin(width)
            spacing: 0
            anchors {
                left: parent ? parent.left : undefined
                right: parent ? parent.right : undefined
            }

            ScrollBar.vertical: ScrollBar {
                width: 10
            }

            header: Item {
                implicitHeight: 50
                anchors {
                    left: parent ? parent.left : undefined
                    right: parent ? parent.right : undefined
                    leftMargin: groupview.lrMargin
                    rightMargin: groupview.lrMargin
                }
                RowLayout {
                    anchors.fill: parent
                    Label {
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.leftMargin: 10
                        font.bold: true
                        font.pointSize: 14
                        text: dccObj.displayName
                    }

                    Button {
                        id: button
                        checkable: true
                        checked: groupSettings.isEditing
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        Layout.rightMargin: 10
                        text: groupSettings.isEditing ? qsTr("done") : qsTr("edit")
                        font.pointSize: 12
                        background: null
                        textColor: Palette {
                            normal {
                                common: DTK.makeColor(Color.Highlight)
                                crystal: DTK.makeColor(Color.Highlight)
                            }
                        }
                        onCheckedChanged: {
                            groupSettings.isEditing = button.checked
                        }
                    }
                }
            }

            model: settings.userId.length > 0 ? dccData.groupsModel(settings.userId) : 0
            delegate: ItemDelegate {
                implicitHeight: 50
                checkable: false
                enabled: model.groupEnabled
                background: DccItemBackground {
                    backgroundType: DccObject.Normal
                    separatorVisible: true
                }
                anchors {
                    left: parent ? parent.left : undefined
                    right: parent ? parent.right : undefined
                    leftMargin: groupview.lrMargin
                    rightMargin: groupview.lrMargin
                }

                contentItem: RowLayout {
                    EditActionLabel {
                        id: editLabel
                        property bool editAble: model.groupEditAble
                        text: model.display
                        validator: RegularExpressionValidator {
                            // 仅使用字母、数字、下划线或短横线，并且以字母开头
                            regularExpression: /[a-zA-Z][a-zA-Z0-9-_]{0,31}$/
                        }
                        implicitHeight: 40
                        implicitWidth: 200
                        placeholderText: qsTr("Group name")
                        horizontalAlignment: TextInput.AlignLeft | Qt.AlignVCenter
                        editBtn.visible: readOnly && editAble
                                         && !groupSettings.isEditing
                        readOnly: model.display.length > 0

                        onFinished: function () {
                            if (text.length < 1) {
                                text = model.display
                                return
                            }
                            if (model.display.length < 1)
                                dccData.createGroup(text)
                            else
                                dccData.modifyGroup(model.display, text)
                        }
                        onFocusChanged: {
                            if (focus || text.length > 0 || editLabel.readonly)
                                return

                            if (model.display.length < 1) {
                                dccData.requestClearEmptyGroup(settings.userId)
                                return
                            }

                            text = model.display
                        }
                        Component.onCompleted: {
                            if (editLabel.readOnly)
                                return

                            Qt.callLater(function () {
                                editLabel.focus = true
                            })
                        }
                    }

                    ActionButton {
                        id: editButton
                        focusPolicy: Qt.NoFocus
                        icon.width: 18
                        icon.height: 18
                        visible: groupSettings.isEditing ? editLabel.editAble : true
                        icon.name: {
                            if (groupSettings.isEditing) {
                                return "list_delete"
                            }

                            return editButton.checked ? "item_checked" : "item_unchecked"
                        }

                        background: null
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        // list_delete.dci icon has padding 10 (1.10p)
                        Layout.rightMargin: groupSettings.isEditing ? 0 : 10
                        // only checked from groupsChanged
                        checkable: false
                        checked: dccData.groupContains(settings.userId, model.display)
                        onClicked: {
                            if (groupSettings.isEditing) {
                                // delete group
                                dccData.deleteGroup(model.display)
                                groupSettings.isEditing = false
                                return
                            }

                            dccData.setGroup(settings.userId, model.display, !editButton.checked)
                        }
                    }
                }

                corners: getCornersForBackground(index, groupview.count)
            }

            footer: Item {
                implicitHeight: 50
                anchors {
                    left: parent ? parent.left : undefined
                    right: parent ? parent.right : undefined
                    leftMargin: groupview.lrMargin
                    rightMargin: groupview.lrMargin
                }
                RowLayout {
                    anchors.fill: parent
                    Button {
                        Layout.alignment: Qt.AlignRight
                        text: qsTr("Add group")
                        implicitWidth: fm.advanceWidth(text) + 12
                        implicitHeight: 30
                        onClicked: {
                            dccData.requestCreateGroup(settings.userId)
                            groupview.positionViewAtEnd()
                        }
                    }
                }
            }
        }
    }
}
