// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import org.deepin.dtk 1.0

import Qt5Compat.GraphicalEffects

import org.deepin.dcc 1.0

DccObject {

    DccObject {
        name: "grubSettingText"
        parentName: "bootMenu"
        displayName: qsTr("Startup Settings")
        weight: 10
        pageType: DccObject.Item
        page: Label {
            leftPadding: 5
            bottomPadding: 5
            font: DTK.fontManager.t4
            text: dccObj.displayName
        }
    }

    DccObject {
        name: "grubSettingList"
        parentName: "bootMenu"
        weight: 20
        pageType: DccObject.Item
        page: Rectangle {

            readonly property int itemDelegateMinWidth: parent.width
            readonly property int itemDelegateMaxWidth: 600
            readonly property int itemDelegateHeight: 40

            id: grublist
            implicitWidth: parent.width
            implicitHeight: menuViewList.height + 20
            color: "transparent"
            Layout.fillWidth: true

            DropArea {
                anchors.fill: parent
                keys: ["text/uri-list"]

                onDropped: (drop) => {
                    if (drop.hasUrls) {
                        let filePath = drop.urls[0].toString()
                        if (filePath.endsWith(".png") || filePath.endsWith(".jpg") || filePath.endsWith(".jpeg") || filePath.endsWith(".bmp")) {
                            console.log("Image dropped:", filePath)
                            if (filePath.startsWith("file://")) {
                                filePath = filePath.substring(7)
                            }
                            dccData.work().setBackground(filePath)
                        } else {
                            console.log("Dropped file is not a supported image:", filePath)
                        }
                    }
                }
            }

            ItemViewport {
                id: viewport
                anchors.fill: image
                sourceItem: image
                radius: 10
                fixed: true
                hideSource: true
                antialiasing: true
            }

            Image {
                id: image
                source: "file://" + dccData.mode().grubThemePath + "?timestamp=" + Date.now()
                asynchronous: true
                anchors.fill: parent
                width: parent.width
                height: menuViewList.height
                fillMode: Image.PreserveAspectCrop
                clip: true
            }

            Rectangle {
                visible: dccData.mode().grubThemePath === ""
                anchors.fill: image
                color: "black"
                radius: 10
            }

            Column {
                spacing: 5
                leftPadding: 20
                id: menuViewList
                Layout.fillWidth: true
                clip: true

                Label {
                    width: grublist.implicitWidth
                    topPadding: 10
                    bottomPadding: 10
                    leftPadding: 5
                    text: qsTr("You can click the menu to change the default startup items, or drag the image to the window to change the background image.")
                    font: DTK.fontManager.t8
                    color: "white"
                    opacity: 0.7
                }

                // 在图片背景上使用Repeater
                Repeater {
                    id: grubMenuList
                    model: dccData.mode().grubMenuListModel()

                    delegate: Rectangle {
                        width: grublist.width - 30
                        height: 30
                        color: "transparent"
                        Rectangle {
                            id: backgru
                            width: grublist.width - 30
                            height: 30
                            radius: 8
                            color: "#2A000000"
                            visible: true

                            RowLayout {
                                Layout.alignment: Qt.AlignVCenter
                                width: parent.width
                                height: 30

                                Label {
                                    height: 20
                                    Layout.alignment: Qt.AlignLeft
                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter
                                    text: model.text
                                    font: DTK.fontManager.t6
                                    color: "white"
                                }

                                DccCheckIcon {
                                    Layout.alignment: Qt.AlignRight
                                    Layout.rightMargin: 10
                                    checked: model.checkStatus
                                    visible: model.checkStatus
                                    size: 16
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    dccData.work().setDefaultEntry(model.text)
                                }
                            }
                        }

                        GaussianBlur {
                            visible: true
                            anchors.fill: parent
                            source: backgru
                            radius: 50
                            samples: 30
                        }
                    }
                }
            }
        }
    }

    DccObject {
        name: "startDelay"
        parentName: "bootMenu"
        displayName: qsTr("grub start delay")
        weight: 30
        pageType: DccObject.Editor
        backgroundType: DccObject.Normal
        page: Switch {
            Layout.alignment: Qt.AlignRight
            checked: dccData.mode().bootDelay

            onCheckedChanged: {
                if (dccData.mode().bootDelay != checked) {
                    dccData.work().setBootDelay(checked)
                }
            }
        }
    }

    DccObject {
        name: "grubTheme"
        parentName: "bootMenu"
        displayName: qsTr("theme")
        description: qsTr("After turning on the theme, you can see the theme background when you turn on the computer")
        weight: 40
        pageType: DccObject.Editor
        backgroundType: DccObject.Normal
        page: Switch {
            Layout.alignment: Qt.AlignRight
            checked: dccData.mode().themeEnabled

            onCheckedChanged: {
                if (checked != dccData.mode().themeEnabled) {
                    dccData.work().setEnableTheme(checked)
                }
            }
        }
    }

    DccObject {
        name: "bootMenuVerification"
        parentName: "bootMenu"
        visible: !dccData.mode().isCommunitySystem()
        displayName: qsTr("Boot menu verification")
        description: qsTr("After opening, entering the menu editing requires a password.")
        weight: 50
        backgroundType: DccObject.Normal
        pageType: DccObject.Item

        page: RowLayout {
            property var passwordDlgStatus: {"Init" : 0, "Cancel" : 1, "Sure" : 2}
            property int dlgStatus: passwordDlgStatus.Init
            Column {
                spacing: 0
                Label {
                    height: 20
                    text: dccObj.displayName
                    font: DTK.fontManager.t6
                    horizontalAlignment: Qt.AlignLeft
                    verticalAlignment: Qt.AlignVCenter
                    leftPadding: 15
                }

                Row {
                    spacing: 0
                    Label {
                        height: 20
                        text: dccObj.description
                        font: DTK.fontManager.t8
                        horizontalAlignment: Qt.AlignLeft
                        verticalAlignment: Qt.AlignVCenter
                        leftPadding: 15
                        opacity: 0.5
                    }

                    Label {
                        height: 20
                        text: "<a href=\"Change Password\">" + qsTr("Change Password") +"</a>"
                        visible: dccData.mode().grubEditAuthEnabled
                        horizontalAlignment: Qt.AlignLeft
                        verticalAlignment: Qt.AlignVCenter
                        color:"#5A000000"
                        // 超链接点击事件
                        onLinkActivated: function(url) {
                            console.log("点击的链接是: " + url)
                            dlgStatus = passwordDlgStatus.Init
                            passwordDlg.show()
                        }
                    }
                }
            }

            Row {
                Layout.alignment: Qt.AlignRight
                Switch {
                    id: verificationSwitch

                    rightPadding: 7
                    checked: dccData.mode().grubEditAuthEnabled
                    onCheckedChanged: {
                        if (checked && !dccData.mode().grubEditAuthEnabled) {
                            passwordDlg.show()
                            return
                        }
                        if (!checked && dccData.mode().grubEditAuthEnabled) {
                            dccData.work().disableGrubEditAuth()
                            return
                        }
                    }
                }

                DialogWindow {
                    id: passwordDlg
                    width: 360
                    height: 350

                    icon: "preferences-system"
                    flags: Qt.Dialog | Qt.WindowCloseButtonHint
                    modality: Qt.ApplicationModal

                    onVisibleChanged: {
                        if (visible) {
                            newPasswordEdit.text = ""
                            repeatPasswordEdit.text = ""
                            newPasswordEdit.showAlert = false
                            repeatPasswordEdit.showAlert = false
                            submitbtn.enabled = false
                            dlgStatus = passwordDlgStatus.Init
                        }
                    }

                    onClosing: function(close) {
                        close.accepted = true
                        if (dlgStatus !== passwordDlgStatus.Sure) {
                            dlgStatus = passwordDlgStatus.Init
                            dccData.work().resetEditAuthEnabled()
                        }
                    }
                    ColumnLayout {
                        width: parent.width
                        spacing: 0

                        Label {
                            id: passwdTitle
                            Layout.topMargin: 10
                            Layout.bottomMargin: 10
                            height: 20
                            Layout.alignment: Qt.AlignHCenter
                            font: DTK.fontManager.t5
                            text: dccData.mode().grubEditAuthEnabled ? qsTr("Change boot menu verification password") : qsTr("Set the boot menu authentication password")
                        }

                        Label {
                            Layout.alignment: Qt.AlignLeft
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            Layout.leftMargin: 5
                            font: DTK.fontManager.t6
                            text: qsTr("User Name :")
                            Layout.preferredWidth: 50
                        }

                        LineEdit {
                            id: userEdit
                            text: qsTr("root")
                            readOnly: true
                            enabled: false
                            clearButton.visible: true
                            Layout.preferredWidth: parent.width
                        }

                        Label {
                            height: 20
                            topPadding: 10
                            Layout.preferredWidth: 60
                            Layout.alignment: Qt.AlignLeft
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            Layout.leftMargin: 5
                            font: DTK.fontManager.t6
                            text: qsTr("New Password :")
                        }

                        PasswordEdit {
                            id: newPasswordEdit
                            placeholderText: qsTr("Required")
                            Layout.preferredWidth: parent.width
                            height: 30
                            showAlert: false

                            Timer {
                                id: newPasswordAlertTimer
                                interval: 3000
                                repeat: false
                                onTriggered: {
                                    newPasswordEdit.showAlert = false
                                }
                            }

                            onTextChanged: {
                                newPasswordAlertTimer.stop()
                                console.log(" newPasswordEdit text changed ", newPasswordEdit.text)
                                if (newPasswordEdit.text.length === 0) {
                                    submitbtn.enabled = false
                                    if (repeatPasswordEdit.text.length !== 0) {
                                        newPasswordEdit.alertText = qsTr("Password cannot be empty")
                                        newPasswordEdit.showAlert = true
                                        newPasswordAlertTimer.start()
                                    } else {
                                        newPasswordEdit.showAlert = false
                                    }
                                    return
                                }

                                var errorText = dccData.work().verifyPassword(newPasswordEdit.text)
                                console.log(" newPasswordEdit text changed ", errorText)
                                if (errorText.length !== 0) {
                                    newPasswordEdit.alertText = errorText
                                    newPasswordEdit.showAlert = true
                                    newPasswordAlertTimer.start()
                                    submitbtn.enabled = false
                                } else {
                                    newPasswordEdit.showAlert = false
                                    if (repeatPasswordEdit.text.length > 0) {
                                        if (repeatPasswordEdit.text !== newPasswordEdit.text) {
                                            repeatPasswordEdit.alertText = qsTr("Passwords do not match")
                                            repeatPasswordEdit.showAlert = true
                                            repeatPasswordAlertTimer.start()
                                            submitbtn.enabled = false
                                        } else {
                                            repeatPasswordEdit.showAlert = false
                                            submitbtn.enabled = true
                                        }
                                    } else {
                                         submitbtn.enabled = false
                                    }
                                }
                            }
                        }

                        Label {
                            height: 20
                            topPadding: 10
                            Layout.preferredWidth: 60
                            Layout.alignment: Qt.AlignLeft
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            Layout.leftMargin: 5
                            font: DTK.fontManager.t6
                            text: qsTr("Repeat password:")
                        }

                        PasswordEdit {
                            id: repeatPasswordEdit
                            Layout.preferredWidth: parent.width
                            placeholderText: qsTr("Required")
                            height: 30

                            Timer {
                                id: repeatPasswordAlertTimer
                                interval: 3000
                                repeat: false
                                onTriggered: {
                                    repeatPasswordEdit.showAlert = false
                                }
                            }

                            onTextChanged: {
                                repeatPasswordAlertTimer.stop()
                                console.log(" repeatPasswordEdit text changed ", repeatPasswordEdit.text)
                                if (repeatPasswordEdit.text.length === 0) {
                                    submitbtn.enabled = false
                                    if (newPasswordEdit.text.length !== 0) {
                                        repeatPasswordEdit.alertText = qsTr("Password cannot be empty")
                                        repeatPasswordEdit.showAlert = true
                                        repeatPasswordAlertTimer.start()
                                    } else {
                                        repeatPasswordEdit.showAlert = false
                                    }
                                    return
                                }

                                var errorText = dccData.work().verifyPassword(repeatPasswordEdit.text)
                                console.log(" repeatPasswordEdit text changed ", errorText)
                                if (errorText.length !== 0) {
                                    repeatPasswordEdit.alertText = errorText
                                    repeatPasswordEdit.showAlert = true
                                    repeatPasswordAlertTimer.start()
                                    submitbtn.enabled = false
                                } else {
                                    if (repeatPasswordEdit.text !== newPasswordEdit.text) {
                                        repeatPasswordEdit.alertText = qsTr("Passwords do not match")
                                        repeatPasswordEdit.showAlert = true
                                        repeatPasswordAlertTimer.start()
                                        submitbtn.enabled = false
                                    } else {
                                        repeatPasswordEdit.showAlert = false
                                        if (newPasswordEdit.text.length > 0 && !newPasswordEdit.showAlert) {
                                             submitbtn.enabled = true
                                        } else {
                                             submitbtn.enabled = false
                                        }
                                        if (newPasswordEdit.alertText === qsTr("Passwords do not match")) {
                                            newPasswordEdit.showAlert = false
                                        }
                                    }
                                }
                            }
                        }

                        RowLayout {
                            Layout.topMargin: 15
                            Layout.preferredWidth: parent.width
                            Layout.fillWidth: true

                            Button {
                                Layout.alignment: Qt.AlignLeft
                                text: qsTr("Cancel")
                                Layout.preferredWidth: 170
                                onClicked: {
                                    dlgStatus = passwordDlgStatus.Cancel
                                    passwordDlg.close()
                                }
                            }

                            RecommandButton {
                                id: submitbtn
                                text: qsTr("Sure")
                                enabled: false
                                Layout.preferredWidth: 170
                                Layout.alignment: Qt.AlignRight
                                onClicked: {
                                    dlgStatus = passwordDlgStatus.Sure
                                    dccData.work().onSetGrubEditPasswd(newPasswordEdit.text, true)
                                    passwordDlg.close()
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    DccObject {
        name: "startAnimation"
        parentName: "bootMenu"
        displayName: qsTr("Start animation")
        description: qsTr("Adjust the size of the logo animation on the system startup interface")
        weight: 60
        pageType: DccObject.Item
        page: ColumnLayout {
            spacing: 0
            Label {
                leftPadding: 5
                topPadding: 20
                font: DTK.fontManager.t4
                text: dccObj.displayName
            }
            Label {
                leftPadding: 5
                bottomPadding: 5
                font: DTK.fontManager.t6
                text: dccObj.description
                opacity: 0.7
            }
        }
    }

    DccObject {
        name: "checkAnimation"
        parentName: "bootMenu"
        weight: 70
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: Rectangle {
            implicitHeight: aniRoot.height
            Layout.fillWidth: true
            width: parent.width
            radius: 8
            color: "transparent"
            property color baseColor: "#0065FF"

            RowLayout {
                id: aniRoot
                spacing: 0
                Repeater {
                    id: repeater
                    model: dccData.mode().grubAnimationModel()
                    delegate: ItemDelegate {

                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        leftPadding: 10
                        rightPadding: 0
                        topPadding: 20
                        bottomPadding: 0
                        spacing: 0
                        contentFlow: true
                        hoverEnabled: true
                        cascadeSelected: true

                        background: DccItemBackground {
                            backgroundType: DccObject.AutoBg
                           // separatorVisible: true
                        }

                        content: ColumnLayout {
                            spacing: 0
                            Rectangle {
                                id: externalRect
                                width: 228
                                height: 148
                                border.width: model.checkStatus ? 2 : 0
                                border.color: "#6A005BFF"
                                radius: 8

                                color: "transparent"

                                Rectangle {
                                    id: imgRect
                                    anchors.centerIn: parent
                                    width: 220
                                    height: 140
                                    border.width: 0
                                    color: DTK.themeType === ApplicationHelper.LightType ?
                                               Qt.rgba(baseColor.r, baseColor.g, baseColor.a, 0.10) : Qt.rgba(baseColor.r, baseColor.g, baseColor.a, 0.15)

                                    radius: 6

                                    Rectangle {
                                        anchors.fill: parent
                                        anchors.margins: 1  // 内描边的宽度
                                        color: "transparent"
                                        border.color: "#1A000000"
                                        radius: 6
                                        border.width: 1

                                        Image {
                                            id: backgroundImage
                                            anchors.centerIn: parent
                                            source: model.imagePath
                                            width: 130 * model.scale
                                            height: 130 * model.scale
                                            visible: !(model.startAnimation
                                                       && model.checkStatus)
                                            fillMode: Image.PreserveAspectCrop
                                            opacity: model.startAnimation ? 0.4 : 1
                                        }

                                        //模糊效果容器
                                        Rectangle {
                                            width: 220
                                            height: 140
                                            anchors.centerIn: parent
                                            radius: 6 // 设置圆角半径
                                            visible: model.startAnimation
                                                     && model.checkStatus
                                            color: "transparent"

                                            // GaussianBlur 模糊效果
                                            ShaderEffectSource {
                                                id: blurSource
                                                sourceItem: backgroundImage
                                                sourceRect: Qt.rect(0, 0, width,
                                                                    height) // 将背景图作为模糊源
                                            }

                                            GaussianBlur {
                                                anchors.fill: parent
                                                source: blurSource
                                                radius: 20 // 模糊半径，值越大，模糊越强
                                                samples: 16
                                            }

                                            BusyIndicator {
                                                anchors.centerIn: parent
                                                running: model.startAnimation
                                                         && model.checkStatus
                                                visible: model.startAnimation
                                                         && model.checkStatus
                                                implicitWidth: 32
                                                implicitHeight: 32
                                            }
                                        }
                                    }

                                    // 使用 MouseArea 来捕捉点击事件
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            dccData.work().setPlymouthFactor(
                                                        model.plymouthScale)
                                        }
                                    }
                                }

                            }

                            RadioButton {
                                autoExclusive: false
                                text: model.text
                                checked: model.checkStatus
                                enabled: !model.startAnimation
                                padding: 0
                                icon.width: 0
                                icon.height: 0
                                onClicked: {
                                    dccData.work().setPlymouthFactor(
                                                model.plymouthScale)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
