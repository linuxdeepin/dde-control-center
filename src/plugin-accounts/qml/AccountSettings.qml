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
    property bool noQuickLoginChecked: true

    Component.onCompleted: {
        updateLoginSettings()
    }
    
    onUserIdChanged: {
        updateLoginSettings()
    }
    
    function updateLoginSettings() {
        settings.autoLoginChecked = dccData.autoLogin(settings.userId)
        settings.nopasswdLoginChecked = dccData.nopasswdLogin(settings.userId)
        settings.noQuickLoginChecked = dccData.quickLogin(settings.userId)
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
        canSearch: settings.canSearch
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
                            dilogLoader.item.transientParent = DccApp.mainWindow()
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
                        function onQuickLoginChanged(userId, enable) {
                            if (userId === settings.userId) {
                                settings.noQuickLoginChecked = enable
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
                    visible: false
                    anchors.fill: parent
                    width: 100
                    height: 100
                    fillMode: Image.PreserveAspectCrop
                    clip: true
                }

                Rectangle {
                    id: mask
                    radius: width / 2
                    visible: false
                    anchors.fill: parent
                }

                OpacityMask {
                    id: opMask
                    source:image
                    maskSource: mask
                    anchors.fill: image
                }

                Rectangle {
                    id: shadow
                    visible: false
                    anchors.fill: parent
                    // color: Qt.rgba(0, 0, 0, 0.5)
                    gradient: Gradient {
                        GradientStop {  position: 0.0; color: Qt.rgba(0, 0, 0, 0) }
                        GradientStop {  position: 0.7; color: Qt.rgba(0, 0, 0, 0) }
                        GradientStop {  position: 1.0; color: Qt.rgba(0, 0, 0, 1) }
                    }
                }

                OpacityMask {
                    id: shadowOpMask
                    source:shadow
                    maskSource: mask
                    visible: control.hovered
                    anchors.fill: shadow
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
                    color: palette.text
                    elide: Text.ElideRight
                    maximumLineCount: 1
                    Layout.maximumWidth: item.width - 230
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
                    implicitWidth: implicitContentWidth + 20
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
                            cadLoader.item.transientParent = DccApp.mainWindow()
                            cadLoader.item.show()
                        }
                    }
                }
            }
        }
    }

    // 账户信息
    DccTitleObject {
        name: "acountInfosTitle"
        parentName: settings.papaName
        displayName: qsTr("Account Information")
        canSearch: settings.canSearch
        weight: 18
    }
    DccObject {
        name: settings.papaName + "acountInfos"
        parentName: settings.papaName
        description: qsTr("Account name, account fullname, account type")
        canSearch: settings.canSearch
        weight: 20
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: settings.papaName + "acountName"
            parentName: settings.papaName + "acountInfos"
            displayName: qsTr("Account name")
            canSearch: settings.canSearch
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
            canSearch: settings.canSearch
            weight: 20
            pageType: DccObject.Editor
            page: RowLayout {
                property string originalFullName: "" // Store original name here

                EditActionLabel {
                    id: fullNameEdit
                    property bool rightClickPressed: false
                    property bool contextMenuVisible: false
                    property int savedSelectionStart: -1
                    property int savedSelectionEnd: -1
                    property string savedSelectedText: ""
                    implicitWidth: 200
                    text: dccData.fullName(settings.userId)
                    placeholderText: qsTr("Set fullname")
                    horizontalAlignment: TextInput.AlignRight
                    editBtn.visible: readOnly
                    
                    onEditingFinished: {
                        if (readOnly)
                            return
                        
                        if (contextMenuVisible) {
                            return
                        }
                        if (rightClickPressed) {
                            rightClickPressed = false
                            return
                        }
                        
                        if (showAlert)
                            showAlert = false

                        readOnly = true
                        finished()
                    }
                    ToolTip {
                        visible: parent.hovered && parent.readOnly && parent.completeText != "" && (parent.metrics.advanceWidth(parent.completeText) > (parent.width - parent.rightPadding - 10))
                        text: parent.completeText
                    }

                    Component.onCompleted: {
                        completeText = text
                        var elidedText = metrics.elidedText(completeText, Text.ElideRight, width - rightPadding - 10)
                        text = elidedText
                    }

                    Menu {
                        id: contextMenu
                        
                        onAboutToShow: {
                            fullNameEdit.contextMenuVisible = true
                            if (fullNameEdit.savedSelectionStart >= 0 && fullNameEdit.savedSelectionEnd >= 0) {
                                Qt.callLater(function() {
                                    fullNameEdit.select(fullNameEdit.savedSelectionStart, fullNameEdit.savedSelectionEnd)
                                })
                            }
                        }
                        
                        onAboutToHide: {
                            fullNameEdit.contextMenuVisible = false
                            Qt.callLater(function() {
                                fullNameEdit.rightClickPressed = false
                                fullNameEdit.forceActiveFocus()
                                fullNameEdit.savedSelectionStart = -1
                                fullNameEdit.savedSelectionEnd = -1
                                fullNameEdit.savedSelectedText = ""
                            })
                        }
                        
                        Action {
                            text: qsTr("Undo")
                            enabled: fullNameEdit.canUndo && !fullNameEdit.readOnly
                            onTriggered: {
                                fullNameEdit.undo()
                                Qt.callLater(function() {
                                    fullNameEdit.forceActiveFocus()
                                })
                            }
                        }
                        
                        Action {
                            text: qsTr("Redo")
                            enabled: fullNameEdit.canRedo && !fullNameEdit.readOnly
                            onTriggered: {
                                fullNameEdit.redo()
                                Qt.callLater(function() {
                                    fullNameEdit.forceActiveFocus()
                                })
                            }
                        }
                        
                        MenuSeparator {}
                        
                        Action {
                            text: qsTr("Cut")
                            enabled: fullNameEdit.savedSelectedText.length > 0 && !fullNameEdit.readOnly
                            onTriggered: {
                                fullNameEdit.cut()
                                Qt.callLater(function() {
                                    fullNameEdit.forceActiveFocus()
                                })
                            }
                        }
                        
                        Action {
                            text: qsTr("Copy")
                            enabled: fullNameEdit.savedSelectedText.length > 0
                            onTriggered: {
                                fullNameEdit.copy()
                                Qt.callLater(function() {
                                    fullNameEdit.forceActiveFocus()
                                })
                            }
                        }
                        
                        Action {
                            text: qsTr("Paste")
                            enabled: !fullNameEdit.readOnly
                            onTriggered: {
                                fullNameEdit.paste()
                                Qt.callLater(function() {
                                    fullNameEdit.forceActiveFocus()
                                })
                            }
                        }
                        
                        MenuSeparator {}
                        
                        Action {
                            text: qsTr("Select All")
                            enabled: fullNameEdit.text.length > 0
                            onTriggered: {
                                fullNameEdit.selectAll()
                                Qt.callLater(function() {
                                    fullNameEdit.forceActiveFocus()
                                })
                            }
                        }
                    }

                    // 右键菜单处理
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton
                        onPressed: function(mouse) {
                            if (mouse.button === Qt.RightButton && parent.text.length > 0) {
                                parent.savedSelectionStart = parent.selectionStart
                                parent.savedSelectionEnd = parent.selectionEnd  
                                parent.savedSelectedText = parent.selectedText
                                parent.rightClickPressed = true
                            }
                        }
                        onClicked: function(mouse) {
                            if (mouse.button === Qt.RightButton && parent.text.length > 0) {
                                contextMenu.popup()
                                mouse.accepted = true
                            }
                        }
                    }

                    onReadOnlyChanged: {
                        if (rightClickPressed) {
                            return
                        }
                        // Store the original text when editing starts
                        if (!readOnly) {
                            text = completeText
                            originalFullName = completeText
                            rightClickPressed = false
                        }
                    }
                    onTextEdited: {
                        rightClickPressed = false
                        if (showAlert)
                            showAlert = false
                        // validtor can not paste invalid text..
                        var regex = /^[^:]{0,32}$/
                        if (!regex.test(text)) {
                            var filteredText = text
                            filteredText = filteredText.replace(":", "")

                            if (filteredText.length > 32) {
                                showAlert = true
                                alertText = qsTr("The full name is too long")
                                dccData.playSystemSound(14)
                            }

                            // 长度 32
                            filteredText = filteredText.slice(0, 32)
                            text = filteredText
                        }
                    }

                    onFinished: function () {
                        if (rightClickPressed) {
                            fullNameEdit.readOnly = false
                            return
                        }
                        // If text hasn't changed, do nothing
                        if (text === originalFullName) {
                            var elidedText = fullNameEdit.metrics.elidedText(fullNameEdit.completeText, Text.ElideRight, 
                            fullNameEdit.width - fullNameEdit.rightPadding - 10)
                            fullNameEdit.text = elidedText
                            return;
                        }

                        // --- Original validation and saving logic ---
                        let alertMsg = dccData.checkFullname(text)
                        if (alertMsg.length > 0) {
                            showAlert = false
                            showAlert = true
                            alertText = alertMsg
                            readOnly = false
                            return
                        }

                        if (text.trim().length === 0) {
                            dccData.setFullname(settings.userId, "");
                            fullNameEdit.text = "";
                            return;
                        }

                        dccData.setFullname(settings.userId, text)
                    }
                    Connections {
                        target: dccData
                        function onFullnameChanged(userId, fullname) {
                            if (userId === settings.userId) {
                                fullNameEdit.completeText = dccData.fullName(settings.userId)
                                var elidedText = fullNameEdit.metrics.elidedText(fullNameEdit.completeText, Text.ElideRight, 
                                fullNameEdit.width - fullNameEdit.rightPadding - 10)
                                fullNameEdit.text = elidedText
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
            canSearch: settings.canSearch
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

                Connections {
                    target: dccData
                    function onUserTypeChanged(userId, type) {
                        if (userId === settings.userId) {
                            currentIndex = type
                        }
                    }
                }
            }
        }
    }

    // 登陆设置
    DccTitleObject {
        name: settings.papaName + "acountSettingsTitle"
        parentName: settings.papaName
        displayName: qsTr("Login settings")
        canSearch: settings.canSearch
        weight: 28
        visible: acountSettings.visible
    }
    DccObject {
        id: acountSettings
        name: settings.papaName + "acountSettings"
        parentName: settings.papaName
        description: qsTr("quick login, Auto login, login without password")
        canSearch: settings.canSearch
        weight: 30
        pageType: DccObject.Item
        page: DccGroupView {}
        visible: (autoLongin.visible || noPassword.visible || quickLogin.visible) && !DccApp.isTreeland()

        DccObject {
            id: quickLogin
            name: settings.papaName + "quickLogin"
            parentName: settings.papaName + "acountSettings"
            displayName: qsTr("Quick login")
            canSearch: settings.canSearch
            weight: 20
            pageType: DccObject.Editor
            visible: dccData.isQuickLoginVisible
            enabled: dccData.currentUserId() === settings.userId
            page: Switch {
                checked: settings.noQuickLoginChecked
                onCheckedChanged: {
                    if (settings.noQuickLoginChecked != checked)
                        settings.noQuickLoginChecked = checked

                    dccData.setQuickLogin(settings.userId, checked)
                }
            }
        }
        
        DccObject {
            id: autoLongin
            name: settings.papaName + "autoLongin"
            parentName: settings.papaName + "acountSettings"
            displayName: qsTr("Auto login")
            canSearch: settings.canSearch
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
                        awdLoader.item.transientParent = DccApp.mainWindow()
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
            canSearch: settings.canSearch
            weight: 30
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
        canSearch: settings.canSearch
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
        canSearch: settings.canSearch
        weight: 0xFFFF
        pageType: DccObject.Item
        page: RowLayout {
            Button {
                id: deleteBtn
                Layout.alignment: groupSettingsBtn.visible ? Qt.AlignLeft : Qt.AlignRight
                text: qsTr("Delete current account")
                enabled: dccData.isDeleteAble(settings.userId)
                implicitWidth: implicitContentWidth + 20
                implicitHeight: 30
                contentItem: Text {
                    text: deleteBtn.text
                    color: deleteBtn.ColorSelector.controlState === DTK.InactiveState ? "#66FF5736" : "#FF5736"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
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
                        cfdLoader.item.transientParent = DccApp.mainWindow()
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
                implicitWidth: implicitContentWidth + 20
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
        canSearch: settings.canSearch
        weight: 10
        pageType: DccObject.Menu
        page: ListView {
            id: groupview
            property int lrMargin: DccUtils.getMargin(width)
            property int conY: 0
            property bool blockInitialFocus: true
            property bool suppressAutoFocusFirstOnFocus: false
            property bool focusNewlyCreatedItem: false
            spacing: 0
            currentIndex: -1
            activeFocusOnTab: true
            keyNavigationEnabled: true
            clip: false

            cacheBuffer: height * 6
            displayMarginBeginning: height * 2
            displayMarginEnd: height * 2

            onActiveFocusChanged: {
                if (activeFocus && count > 0) {
                    if (focusNewlyCreatedItem) {
                        return
                    }
                    if (blockInitialFocus) {
                        if (model && model.isCreatingGroup) {
                            blockInitialFocus = false
                            return
                        }
                        blockInitialFocus = false
                        focus = false
                        return
                    }
                    if (suppressAutoFocusFirstOnFocus) {
                        suppressAutoFocusFirstOnFocus = false
                        return
                    }
                    currentIndex = 0
                    positionViewAtIndex(0, ListView.Beginning)
                }
            }

            onCurrentIndexChanged: {
                if (activeFocus && currentItem) {
                    currentItem.forceActiveFocus()
                }
            }

            function qmlListModelUpdata() {
                if (model && model.isCreatingGroup) {
                    model.setCreatingGroup(false)
                    Qt.callLater(function () {
                        groupview.positionViewAtEnd()
                        if (groupview.focusNewlyCreatedItem && groupview.count > 0) {
                            groupview.currentIndex = groupview.count - 1
                            if (groupview.currentItem && groupview.currentItem.forceActiveFocus) {
                                groupview.currentItem.forceActiveFocus()
                            }
                            groupview.focusNewlyCreatedItem = false
                        }
                    })
                } else {
                    groupview.contentY = conY
                }
            }

            Connections {
                target: model
                function onGroupsUpdated() {
                    qmlListModelUpdata()
                }
            }

            onContentYChanged: {
                if(contentY != -50) {
                    conY = contentY
                }
            }
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
                    DccLabel {
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.leftMargin: 10
                        font.pixelSize: DTK.fontManager.t5.pixelSize
                        font.weight: 700
                        text: dccObj.displayName
                    }

                    Button {
                        id: button
                        checkable: true
                        checked: groupSettings.isEditing
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        Layout.rightMargin: 10
                        text: groupSettings.isEditing ? qsTr("done") : qsTr("edit")
                        font: DTK.fontManager.t8
                        background: null
                        focusPolicy: Qt.NoFocus
                        textColor: Palette {
                            normal {
                                common: DTK.makeColor(Color.Highlight)
                                crystal: DTK.makeColor(Color.Highlight)
                            }
                        }
                        onPressed: {
                            groupview.suppressAutoFocusFirstOnFocus = true
                        }
                        onCheckedChanged: {
                            groupSettings.isEditing = button.checked
                        }
                    }
                }
            }

            model: settings.userId.length > 0 ? dccData.groupsModel(settings.userId) : 0
            delegate: ItemDelegate {
                id: itemDelegate
                implicitHeight: 36
                padding: 0
                checkable: false
                clip: false
                z: editLabel.showAlert ? 100 : 1

                focusPolicy: Qt.StrongFocus
                activeFocusOnTab: false

                Keys.onPressed: function(event) {
                    if (!model.groupEnabled) {
                        if (event.key === Qt.Key_Up) {
                            if (groupview.currentIndex > 0) {
                                groupview.currentIndex = groupview.currentIndex - 1
                                groupview.positionViewAtIndex(groupview.currentIndex, ListView.Contain)
                            }
                            event.accepted = true
                            return
                        } else if (event.key === Qt.Key_Down) {
                            if (groupview.currentIndex < groupview.count - 1) {
                                groupview.currentIndex = groupview.currentIndex + 1
                                groupview.positionViewAtIndex(groupview.currentIndex, ListView.Contain)
                            }
                            event.accepted = true
                            return
                        }
                        event.accepted = true
                        return
                    }
                    if (editLabel.readOnly && (event.key === Qt.Key_Tab || event.key === Qt.Key_Backtab)) {
                        groupview.focus = false
                        groupview.currentIndex = -1
                        event.accepted = true
                        return
                    }
                    if (!editLabel.readOnly) {
                        return
                    }
                    if (event.key === Qt.Key_Up) {
                        if (groupview.currentIndex > 0) {
                            groupview.currentIndex = groupview.currentIndex - 1
                            groupview.positionViewAtIndex(groupview.currentIndex, ListView.Contain)
                        }
                        event.accepted = true
                    } else if (event.key === Qt.Key_Down) {
                        if (groupview.currentIndex < groupview.count - 1) {
                            groupview.currentIndex = groupview.currentIndex + 1
                            groupview.positionViewAtIndex(groupview.currentIndex, ListView.Contain)
                        }
                        event.accepted = true
                    } else if (event.key === Qt.Key_Space || event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                        if (!groupSettings.isEditing && model.groupEnabled) {
                            dccData.setGroup(settings.userId, model.display, !editButton.checked)
                            groupview.currentIndex = -1
                            groupview.focus = false
                        }
                        event.accepted = true
                    }
                }

                property var editTextWidth: itemDelegate.width - editButton.width - rightPadding - 80
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

                onWidthChanged: {
                    if (editLabel.readOnly) {
                        var elidedText = editLabel.metrics.elidedText(editLabel.completeText, Text.ElideRight, editTextWidth)
                        editLabel.text = elidedText
                        return
                    }
                }

                contentItem: RowLayout {
                    spacing: 0
                    Item {
                        id: editContainer
                        Layout.fillWidth: !editLabel.readOnly
                        Layout.alignment: Qt.AlignVCenter
                        Layout.leftMargin: 14
                        implicitHeight: 36
                        implicitWidth: Math.min(editLabel.metrics.advanceWidth(editLabel.text) + editButton.width + 20,
                                         groupview.width - editButton.width - 30)
                        
                        EditActionLabel {
                            id: editLabel
                            property bool editAble: model.groupEditAble
                            property string lastValidText: ""
                            property bool isRestoring: false
                            property string originalGroupName: ""
                            anchors.fill: parent
                            text: model.display
                            completeText: model.display
                            rightPadding: editButton.width + 10
                            placeholderText: qsTr("Group name")
                            horizontalAlignment: TextInput.AlignLeft
                            verticalAlignment: TextInput.AlignVCenter
                            editBtn.visible: readOnly && editAble
                                             && !groupSettings.isEditing
                            readOnly: model.display.length > 0
                            ToolTip {
                                visible: parent.hovered && parent.readOnly && parent.completeText != "" && (parent.metrics.advanceWidth(parent.completeText) > (parent.width - parent.rightPadding - 10))
                                text: parent.completeText
                            }

                            // Monitor model.display changes to update elided text
                            onCompleteTextChanged: {
                                if (readOnly && completeText.length > 0) {
                                    var elidedText = metrics.elidedText(completeText, Text.ElideRight, editTextWidth)
                                    text = elidedText
                                }
                            }
                        
                        onReadOnlyChanged: {
                            if (!readOnly) {
                                groupview.suppressAutoFocusFirstOnFocus = true
                                text = completeText
                                lastValidText = model.display
                                originalGroupName = model.display
                            }
                        }
                        
                        Connections {
                            target: dccData
                            function onGroupsUpdateFailed(groupName) {
                                if (groupName === editLabel.originalGroupName) {
                                    editLabel.completeText = editLabel.originalGroupName
                                    var elidedText = editLabel.metrics.elidedText(editLabel.originalGroupName, Text.ElideRight, editTextWidth)
                                    editLabel.text = elidedText
                                }
                            }
                        }
                        
                        onTextChanged: {
                            if (readOnly) {
                                return
                            }
                            if (isRestoring) {
                                isRestoring = false
                                return
                            }
                            
                            if (showAlert)
                                showAlert = false
                            
                            if (text.length < 1 && lastValidText.length > 0 && !readOnly) {
                                Qt.callLater(function() {
                                    editLabel.forceActiveFocus()
                                })
                            }
                            
                            var isNewGroup = (model.display.length === 0)
                            
                            if (text.length > 32) {
                                showAlert = true
                                alertText = qsTr("Group names should be no more than 32 characters")
                                dccData.playSystemSound(14)
                                isRestoring = true
                                text = lastValidText
                                return
                            }
                        
                            var numbersOnlyRegex = /^[0-9]+$/
                            if (text.length > 0 && numbersOnlyRegex.test(text)) {
                                showAlert = true
                                alertText = qsTr("Group names cannot only have numbers")
                                dccData.playSystemSound(14)
                                isRestoring = true
                                text = lastValidText
                                return
                            }
                                
                            var validFormatRegex = /^[a-zA-Z][a-zA-Z0-9-_]*$/
                            if (text.length > 0 && !validFormatRegex.test(text) && model.display != "_ssh") {
                                showAlert = true
                                alertText = qsTr("Use letters,numbers,underscores and dashes only, and must start with a letter")
                                dccData.playSystemSound(14)
                                isRestoring = true
                                text = lastValidText
                                return
                            }
                            
                            lastValidText = text
                        }
                        
                        onFinished: function () {
                            var wasNewGroup = (model.display.length < 1)
                            if (text.length < 1) {
                                if (model.display.length < 1) {
                                    dccData.requestClearEmptyGroup(settings.userId)
                                } else {
                                    var elidedText = metrics.elidedText(model.display, Text.ElideRight, editTextWidth)
                                    text = elidedText
                                    readOnly = true
                                }
                                return
                            }
                            
                            if (dccData.groupExists(text) && text !== model.display) {
                                showAlert = true
                                alertText = qsTr("The group name has been used")
                                return
                            }
                            
                            if (text === model.display) {
                                completeText = text
                                var elidedText = metrics.elidedText(completeText, Text.ElideRight, editTextWidth)
                                text = elidedText
                                readOnly = true
                                return
                            }
                            
                            if (model.display.length < 1)
                                dccData.createGroup(text)
                            else
                                dccData.modifyGroup(model.display, text)
                            
                            completeText = text
                            var elidedText = metrics.elidedText(completeText, Text.ElideRight, editTextWidth)
                            text = elidedText
                            readOnly = true

                            // 当通过回车结束新建分组的编辑时，清空 ListView 的焦点与选中
                            if (wasNewGroup) {
                                groupview.currentIndex = -1
                                groupview.focus = false
                            }
                        }
                        onFocusChanged: {
                            if (focus || text.length > 0 || editLabel.readOnly)
                                return

                            if (model.display.length < 1) {
                                dccData.requestClearEmptyGroup(settings.userId)
                                return
                            }

                            text = model.display
                        }
                        Component.onCompleted: {
                            completeText = model.display
                            lastValidText = model.display
                            originalGroupName = model.display  // Initialize original name
                            
                            if (editLabel.readOnly) {
                                var elidedText = metrics.elidedText(completeText, Text.ElideRight, editTextWidth)
                                text = elidedText
                                return
                            }

                            Qt.callLater(function () {
                                editLabel.focus = true
                            })
                        }
                    }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                        implicitHeight: 36
                        
                        MouseArea {
                            anchors.fill: parent
                            visible: !groupSettings.isEditing
                            enabled: model.groupEnabled
                            acceptedButtons: Qt.AllButtons
                            onClicked: {
                                dccData.setGroup(settings.userId, model.display, !editButton.checked)
                            }
                        }
                    }

                    ActionButton {
                        id: editButton
                        focusPolicy: Qt.NoFocus
                        icon.width: 16
                        icon.height: 16
                        visible: groupSettings.isEditing ? editLabel.editAble : editLabel.readOnly
                        enabled: model.groupEnabled
                        icon.name: {
                            if (groupSettings.isEditing) {
                                return "list_delete"
                            }

                            return editButton.checked ? "item_checked" : "radio_unchecked"
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

                Item {
                    anchors.fill: parent
                    visible: !model.groupEnabled
                    z: 10000
                    focus: true
                    Keys.onPressed: function(event) {
                        if (event.key === Qt.Key_Up) {
                            if (groupview.currentIndex > 0) {
                                groupview.currentIndex = groupview.currentIndex - 1
                                groupview.positionViewAtIndex(groupview.currentIndex, ListView.Contain)
                            }
                            event.accepted = true
                            return
                        } else if (event.key === Qt.Key_Down) {
                            if (groupview.currentIndex < groupview.count - 1) {
                                groupview.currentIndex = groupview.currentIndex + 1
                                groupview.positionViewAtIndex(groupview.currentIndex, ListView.Contain)
                            }
                            event.accepted = true
                            return
                        }
                        event.accepted = true
                    }
                    Keys.onReleased: function(event) { event.accepted = true }
                    Keys.onShortcutOverride: function(event) { event.accepted = true }

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.AllButtons
                        hoverEnabled: true
                        preventStealing: true
                        onPressed: function(mouse) { mouse.accepted = true }
                        onReleased: function(mouse) { mouse.accepted = true }
                        onClicked: function(mouse) { mouse.accepted = true }
                        onWheel: function(wheel) { wheel.accepted = true }
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
                        implicitWidth: implicitContentWidth + 20
                        implicitHeight: 30
                        focusPolicy: Qt.NoFocus
                        onClicked: {
                            groupview.suppressAutoFocusFirstOnFocus = true
                            groupview.focusNewlyCreatedItem = true
                            dccData.requestCreateGroup(settings.userId)
                            groupview.positionViewAtEnd()
                        }
                    }
                }
            }
        }
    }
}
