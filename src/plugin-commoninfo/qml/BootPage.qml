// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtQuick.Effects
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
            leftPadding: 14
            font.pixelSize: DTK.fontManager.t5.pixelSize
            font.weight: 500
            color: DTK.themeType === ApplicationHelper.LightType ?
                    Qt.rgba(0, 0, 0, 1) : Qt.rgba(1, 1, 1, 1)
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
            Layout.topMargin: 0

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

            Image {
                id: image
                source: "file://" + dccData.mode().grubThemePath + "?timestamp=" + Date.now()
                asynchronous: true
                anchors.fill: parent
                width: parent.width
                height: menuViewList.height
                fillMode: Image.PreserveAspectCrop
                mipmap: true
                
                layer.enabled: true
                layer.effect: MultiEffect {
                    maskEnabled: true
                    maskSource: imageMask
                    antialiasing: true
                    maskThresholdMin: 0.5
                    maskSpreadAtMin: 1.0
                }
            }

            Rectangle {
                id: overlayLayer
                anchors.fill: image
                color: "#33000000"
                radius: 10
                antialiasing: true
                smooth: true
                
                layer.enabled: true
                layer.effect: MultiEffect {
                    maskEnabled: true
                    maskSource: imageMask
                    antialiasing: true
                    maskThresholdMin: 0.5
                    maskSpreadAtMin: 1.0
                }
            }

            Item {
                id: imageMask
                anchors.fill: image
                layer.enabled: true
                visible: false
                
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 0.5
                    radius: 10
                    color: "white"
                }
            }

            Rectangle {
                visible: dccData.mode().grubThemePath === ""
                anchors.fill: image
                color: "black"
                radius: 10
                antialiasing: true
                smooth: true
            }

            Column {
                spacing: 5
                leftPadding: 10
                rightPadding: 10
                id: menuViewList
                Layout.fillWidth: true
                width: parent.width
                height: childrenRect.height
                clip: false

                Label {
                    id: grubSettingText
                    width: grublist.implicitWidth - 30
                    topPadding: 10
                    bottomPadding: 5
                    leftPadding: 10
                    elide: Text.ElideRight
                    text: qsTr("You can click the menu to change the default startup items, or drag the image to the window to change the background image.")
                    font: DTK.fontManager.t8
                    color: "white"
                    opacity: 0.7

                    TextMetrics {
                        id: textMetrics
                        font: grubSettingText.font
                        text: grubSettingText.text
                    }

                    ToolTip {
                        text: grubSettingText.text
                        visible: grubSettingMouseArea.containsMouse && textMetrics.width > grubSettingText.width
                    }

                    MouseArea {
                        id: grubSettingMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }

                // 在图片背景上使用Repeater
                Repeater {
                    id: grubMenuList
                    model: dccData.mode().grubMenuListModel()

                    delegate: Rectangle {
                        width: grublist.width - 20
                        height: 30
                        color: "transparent"
                        
                        Rectangle {
                            id: backgru
                            property bool isHovered: false

                            width: grublist.width - 20
                            height: 30
                            radius: 8
                            color: {
                                if (model.checkStatus) {
                                    return "#34FFFFFF"
                                } else if (isHovered) {
                                    return "#1AFFFFFF"
                                } else {
                                    return "transparent"
                                }
                            }

                            RowLayout {
                                Layout.alignment: Qt.AlignVCenter
                                width: parent.width
                                height: 30

                                Label {
                                    height: 20
                                    Layout.alignment: Qt.AlignLeft
                                    Layout.leftMargin: 10
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
                                hoverEnabled: true
                                onClicked: {
                                    dccData.work().setDefaultEntry(model.text)
                                }
                                onEntered: {
                                    parent.isHovered = true
                                }
                                onExited: {
                                    parent.isHovered = false
                                }
                            }
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
                Layout.topMargin: 4
                Layout.bottomMargin: 9
                Layout.fillWidth: true
                Layout.maximumWidth: parent.width - 100
                
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
                    width: Math.min(parent.width, descriptionTextMetrics.width + changePasswordLabel.implicitWidth + 15)
                    clip: true
                    
                    Label {
                        id: descriptionLabel
                        height: 20
                        text: dccObj.description
                        font: DTK.fontManager.t10
                        horizontalAlignment: Qt.AlignLeft
                        verticalAlignment: Qt.AlignVCenter
                        leftPadding: 15
                        opacity: 0.5
                        elide: Text.ElideRight
                        width: changePasswordLabel.visible ? 
                               Math.max(0, Math.min(descriptionTextMetrics.width + 15, parent.parent.width - changePasswordLabel.implicitWidth - 30)) :
                               Math.max(0, Math.min(descriptionTextMetrics.width + 15, parent.parent.width))
                        
                        TextMetrics {
                            id: descriptionTextMetrics
                            font: descriptionLabel.font
                            text: descriptionLabel.text
                        }
                        
                        ToolTip {
                            text: dccObj.description
                            visible: descriptionMouseArea.containsMouse && descriptionTextMetrics.width > (descriptionLabel.width - 15)
                        }
                        
                        MouseArea {
                            id: descriptionMouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                        }
                    }

                    Label {
                        id: changePasswordLabel
                        height: 20
                        text: "<a href=\"Change Password\">" + qsTr("Change Password") +"</a>"
                        visible: dccData.mode().grubEditAuthEnabled
                        horizontalAlignment: Qt.AlignLeft
                        verticalAlignment: Qt.AlignVCenter
                        font: DTK.fontManager.t10
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
                    height: columnLayout.implicitHeight + 50

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
                        id: columnLayout
                        width: parent.width
                        spacing: 0

                        Label {
                            id: passwdTitle
                            Layout.topMargin: 10
                            Layout.bottomMargin: 10
                            Layout.alignment: Qt.AlignHCenter
                            Layout.preferredWidth: parent.width - 20
                            Layout.leftMargin: 10
                            Layout.rightMargin: 10
                            font: DTK.fontManager.t5
                            text: dccData.mode().grubEditAuthEnabled ? qsTr("Change boot menu verification password") : qsTr("Set the boot menu authentication password")
                            wrapMode: Text.Wrap
                            horizontalAlignment: Text.AlignHCenter
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
                            canCopy: false
                            canCut: false

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
                            canCopy: false
                            canCut: false

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
                            Layout.bottomMargin: 6
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
        onParentItemChanged: item => { if (item) item.bottomPadding = 0 }
    }

    DccObject {
        name: "startAnimation"
        parentName: "bootMenu"
        displayName: qsTr("Start animation")
        description: qsTr("Adjust the size of the logo animation on the system startup interface")
        weight: 60
        pageType: DccObject.Item
        page: ColumnLayout {
            Layout.topMargin: 0
            Layout.bottomMargin: 0
            width: parent.width - 28
            spacing: 0
            Label {
                leftPadding: 14
                topPadding: 10
                font.pixelSize: DTK.fontManager.t5.pixelSize
                font.weight: 500
                color: DTK.themeType === ApplicationHelper.LightType ?
                        Qt.rgba(0, 0, 0, 1) : Qt.rgba(1, 1, 1, 1)
                text: dccObj.displayName
            }
            Label {
                id: startAnimationDescriptionLabel
                leftPadding: 14
                bottomPadding: 5
                font: DTK.fontManager.t8
                text: dccObj.description
                opacity: 0.7
                elide: Text.ElideRight
                Layout.fillWidth: true
                
                property bool showToolTip: false
                
                TextMetrics {
                    id: startAnimationDescriptionTextMetrics
                    font: DTK.fontManager.t8
                    text: dccObj.description
                }
                
                ToolTip {
                    text: dccObj.description
                    visible: startAnimationDescriptionLabel.showToolTip && startAnimationDescriptionTextMetrics.width > parent.width
                }
                
                MouseArea {
                    id: startAnimationDescriptionMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    
                    onEntered: {
                        if (startAnimationDescriptionTextMetrics.width > parent.width) {
                            startAnimationDescriptionLabel.showToolTip = true
                        }
                    }
                    
                    onExited: {
                        startAnimationDescriptionLabel.showToolTip = false
                    }
                }
            }
        }
        onParentItemChanged: item => { 
            if (item) {
                item.topPadding = 0
                item.bottomPadding = 0
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
                                border.color: parent.palette.highlight
                                radius: 10

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
                                        color: "transparent"
                                        border.color: DTK.themeType === ApplicationHelper.LightType ? "#0D000000" : "#1AFFFFFF"
                                        radius: 6
                                        border.width: 1

                                        Image {
                                            id: backgroundImage
                                            anchors.centerIn: parent
                                            source: model.imagePath
                                            width: 130 * model.scale
                                            height: 130 * model.scale
                                            visible: !(model.startAnimation && model.checkStatus) || blurContainer.grabbing
                                            fillMode: Image.PreserveAspectCrop
                                            opacity: model.startAnimation ? 0.4 : 1
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

                                //模糊效果容器
                                Rectangle {
                                    id: blurContainer
                                    anchors.centerIn: parent
                                    width: 220
                                    height: 140
                                    radius: 6
                                    visible: model.startAnimation && model.checkStatus
                                    color: "transparent"

                                    property alias grabbedImage: grabbedImageItem
                                    property bool grabbing: false

                                    Image {
                                        id: grabbedImageItem
                                        anchors.fill: parent
                                        fillMode: Image.PreserveAspectCrop
                                        visible: false
                                    }

                                    GaussianBlur {
                                        anchors.fill: parent
                                        source: grabbedImageItem
                                        radius: 25 // 模糊半径，值越大，模糊越强
                                        samples: 16
                                    }

                                    BusyIndicator {
                                        anchors.centerIn: parent
                                        running: model.startAnimation && model.checkStatus
                                        visible: model.startAnimation && model.checkStatus
                                        implicitWidth: 32
                                        implicitHeight: 32
                                    }

                                    onVisibleChanged: {
                                        if (visible && model.startAnimation && model.checkStatus) {
                                            grabbing = true
                                            imgRect.grabToImage(function(result) {
                                                grabbedImageItem.source = result.url
                                                grabbing = false
                                            })
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
