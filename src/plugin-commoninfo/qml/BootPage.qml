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
    property bool isReset: true
    DccObject {
        name: "grubSettingText"
        parentName: "bootMenu"
        displayName: qsTr("Startup Settings")
        weight: 10
        hasBackground: false
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
        hasBackground: false
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
                source: "qrc:/icons/deepin/builtin/icons/dde-grub-background.jpg"
                asynchronous: true
                anchors.fill: parent
                width: parent.width
                height: menuViewList.height
                fillMode: Image.PreserveAspectCrop
                clip: true
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
        hasBackground: true
        page: Switch {
            Layout.alignment: Qt.AlignRight
            checked: dccData.mode().bootDelay

            onCheckedChanged: {
                dccData.work().setBootDelay(checked)
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
        hasBackground: true
        page: Switch {
            Layout.alignment: Qt.AlignRight
            checked: dccData.mode().themeEnabled

            onCheckedChanged: {
                dccData.work().setEnableTheme(checked)
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
        hasBackground: true
        pageType: DccObject.Editor
        page: Row {
            Switch {
                id: verificationSwitch
                Layout.alignment: Qt.AlignRight
                checked: dccData.mode().grubEditAuthEnabled

                onCheckedChanged: {
                    if (checked) {
                        passwordDlg.show()
                    } else {
                        if (!isReset) {
                            dccData.work().disableGrubEditAuth(checked)
                        }
                    }
                }
            }

            DialogWindow {
                id: passwordDlg
                width: 420
                height: 276

                icon: "preferences-system"
                flags: Qt.Dialog | Qt.WindowCloseButtonHint
                modality: Qt.ApplicationModal

                // 弹窗关闭时触发的事件
                onClosing: {
                    if (isReset) {
                        verificationSwitch.checked = !verificationSwitch.checked
                    }

                    close.accepted = true
                }
                ColumnLayout {
                    width: parent.width
                    spacing: 5

                    Label {
                        Layout.topMargin: 10
                        Layout.bottomMargin: 10
                        height: 20
                        Layout.alignment: Qt.AlignHCenter
                        font: DTK.fontManager.t5
                        text: qsTr("Change boot menu verification password")
                    }
                    RowLayout {
                        spacing: 0
                        Layout.preferredWidth: parent.width
                        Layout.fillWidth: true
                        Label {
                            Layout.alignment: Qt.AlignLeft
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            font: DTK.fontManager.t6
                            text: qsTr("User Name")
                            Layout.preferredWidth: 50
                        }

                        LineEdit {
                            id: userEdit
                            placeholderText: qsTr("roots")
                            Layout.preferredWidth: 280
                            Layout.alignment: Qt.AlignRight
                        }
                    }

                    RowLayout {
                        spacing: 0
                        Layout.preferredWidth: parent.width
                        Layout.fillWidth: true

                        Label {
                            height: 30
                            Layout.preferredWidth: 60
                            Layout.alignment: Qt.AlignLeft
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            font: DTK.fontManager.t6
                            text: qsTr("New Password")
                        }

                        PasswordEdit {
                            id: newPasswordEdit
                            placeholderText: qsTr("Required")
                            Layout.alignment: Qt.AlignRight
                            Layout.preferredWidth: 280
                            height: 30
                            showAlert: false

                            onTextChanged: {
                                console.log(" newPasswordEdit text changed ", newPasswordEdit.text)
                                if (newPasswordEdit.text.length === 0) {
                                    if (repeatPasswordEdit.text.length !== 0) {
                                        newPasswordEdit.alertText = qsTr("Password cannot be empty")
                                        newPasswordEdit.showAlert = true
                                    }
                                    return
                                }

                                var errorText = dccData.work().verifyPassword(newPasswordEdit.text)
                                console.log(" newPasswordEdit text changed ", errorText)
                                if (errorText.length !== 0) {
                                    newPasswordEdit.alertText = errorText
                                    newPasswordEdit.showAlert = true
                                    submitbtn.enabled = false
                                } else {
                                    if (repeatPasswordEdit.text !== newPasswordEdit.text) {
                                        newPasswordEdit.alertText = qsTr("Passwords do not match")
                                    }
                                    var isAlert = (repeatPasswordEdit.text.length !== 0 && repeatPasswordEdit.text === newPasswordEdit.text)

                                    if (isAlert) {
                                        repeatPasswordEdit.showAlert = false
                                    }
                                    newPasswordEdit.showAlert = isAlert
                                    submitbtn.enabled = isAlert
                                }
                            }
                        }
                    }

                    RowLayout {
                        spacing: 0
                        Layout.preferredWidth: parent.width
                        Layout.fillWidth: true

                        Label {
                            height: 30
                            Layout.preferredWidth: 60
                            Layout.alignment: Qt.AlignLeft
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            font: DTK.fontManager.t6
                            text: qsTr("Repeat password")
                        }

                        PasswordEdit {
                            id: repeatPasswordEdit
                            Layout.alignment: Qt.AlignRight
                            Layout.preferredWidth: 280
                            placeholderText: qsTr("Required")
                            height: 30

                            onTextChanged: {
                                console.log(" repeatPasswordEdit text changed ",
                                    repeatPasswordEdit.text)
                                if (repeatPasswordEdit.text.length === 0) {
                                    if (newPasswordEdit.text.length !== 0) {
                                        repeatPasswordEdit.alertText = qsTr("Password cannot be empty")
                                        repeatPasswordEdit.showAlert = true
                                    }
                                    return
                                }

                                var errorText = dccData.work().verifyPassword(repeatPasswordEdit.text)
                                console.log(" repeatPasswordEdit text changed ", errorText)
                                if (errorText.length !== 0) {
                                    repeatPasswordEdit.alertText = errorText
                                    repeatPasswordEdit.showAlert = true
                                    submitbtn.enabled = false
                                } else {
                                    if (repeatPasswordEdit.text !== newPasswordEdit.text) {
                                        repeatPasswordEdit.alertText = qsTr("Passwords do not match")
                                    }

                                    var isAlert = (repeatPasswordEdit.text.length !== 0 && repeatPasswordEdit.text === newPasswordEdit.text)

                                    if (isAlert) {
                                        newPasswordEdit.showAlert = false
                                    }
                                    repeatPasswordEdit.showAlert = !isAlert
                                    submitbtn.enabled = isAlert
                                }
                            }
                        }
                    }

                    RowLayout {
                        Layout.topMargin: 10
                        Layout.preferredWidth: parent.width
                        Layout.fillWidth: true

                        Button {
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Cancel")
                            Layout.preferredWidth: 176
                            onClicked: {
                                isReset = true
                                passwordDlg.close()
                            }
                        }

                        RecommandButton {
                            id: submitbtn
                            text: qsTr("Sure")
                            enabled: false
                            Layout.preferredWidth: 176
                            Layout.alignment: Qt.AlignRight
                            onClicked: {
                                isReset = false
                                dccData.work().onSetGrubEditPasswd(newPasswordEdit.text, true)
                                passwordDlg.close()
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
        hasBackground: false
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
        hasBackground: true
        pageType: DccObject.Item
        page: Rectangle {
            implicitHeight: aniRoot.height
            Layout.fillWidth: true
            width: parent.width
            radius: 8

            Row {
                id: aniRoot
                width: parent.width
                topPadding: 20
                spacing: 15
                leftPadding: 5
                bottomPadding: 20

                Repeater {
                    model: dccData.mode().grubAnimationModel()
                    delegate: Column {
                        spacing: 5
                        Rectangle {
                            id: externalRect
                            width: 228
                            height: 148
                            border.width: model.checkStatus ? 2 : 0
                            border.color: "#6A005BFF"
                            radius: 8

                            Rectangle {
                                id: imgRect
                                anchors.centerIn: parent
                                width: 220
                                height: 140
                                border.width: 1
                                border.color: "#1A000000"
                                color: "#1A0065FF"
                                radius: 6

                                Image {
                                    id: backgroundImage
                                    anchors.centerIn: parent
                                    source: model.imagePath
                                    width: 300 * model.scale
                                    height: 100 * model.scale
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
                                    color: "#1A0065FF"

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

                        RadioButton {
                            height: 18
                            autoExclusive: false
                            text: model.text
                            checked: model.checkStatus
                            enabled: !model.startAnimation
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
