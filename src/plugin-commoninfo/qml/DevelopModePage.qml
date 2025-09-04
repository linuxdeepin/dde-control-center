// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.15
import org.deepin.dtk 1.0 as D

import org.deepin.dcc 1.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import Qt.labs.platform 1.1
import org.deepin.dtk.style 1.0 as DS

DccObject {
    DccObject {
        name: "developTitle"
        parentName: "developerMode"
        displayName: qsTr("Root Access")
        weight: 10
        visible: !dccData.mode().isCommunitySystem()
        pageType: DccObject.Item
        page: Label {
            leftPadding: 15
            bottomPadding: -2
            text: dccObj.displayName
            font.pixelSize: D.DTK.fontManager.t5.pixelSize
            font.weight: 500
            color: D.DTK.themeType === D.ApplicationHelper.LightType ?
                    Qt.rgba(0, 0, 0, 1) : Qt.rgba(1, 1, 1, 1)
        }
    }

    DccObject {
        name: "developerModeSetting"
        parentName: "developerMode"
        weight: 20
        visible: !dccData.mode().isCommunitySystem()
        pageType: DccObject.Item
        page: DccGroupView {

            Layout.topMargin: 0
        }

        DccObject {
            name: "developerModeStatus"
            parentName: "developerModeSetting"
            displayName: qsTr("Request Root Access")
            pageType: DccObject.Item
            backgroundType: DccObject.Normal
            weight: 20
            page: RowLayout {
                id: root
                Layout.topMargin: 5
                ColumnLayout {
                    spacing: 2
                    Layout.leftMargin: 15
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                    Label {
                        text: dccObj.displayName
                        font: D.DTK.fontManager.t6
                    }

                    Label {
                        horizontalAlignment: Text.AlignLeft
                        wrapMode: Text.WordWrap
                        text: qsTr("After entering the developer mode, you can obtain root permissions, but it may also damage the system integrity, so please use it with caution.")
                        font: D.DTK.fontManager.t10
                        opacity: 0.5

                        Layout.fillWidth: true
                    }
                }

                Label {
                    Layout.alignment: Qt.AlignRight
                    Layout.rightMargin: 10
                    visible: dccData.mode().developerModeState || dccData.mode().isDeveloperMode
                    text: qsTr("Allowed")
                    font: D.DTK.fontManager.t8
                }

                Button {
                    Layout.alignment: Qt.AlignRight
                    Layout.rightMargin: 10
                    implicitWidth: 50

                    visible: !(dccData.mode().developerModeState || dccData.mode().isDeveloperMode)
                    text: qsTr("Enter")

                    onClicked: {
                        developDlg.show()
                    }
                }

                Loader {
                    id: loader
                    active: dccData.mode().needShowModalDialog
                    sourceComponent: Window {
                        id: modalDialog
                        flags: Qt.Window
                        modality: Qt.ApplicationModal
                        color: "transparent"
                        opacity: 0.0
                    }
                    onLoaded: {
                        item.show()
                    }
                }

                D.DialogWindow {
                    id: developDlg
                    width: 360
                    height: 360

                    minimumWidth: 360
                    minimumHeight: 360
                    maximumWidth: 360
                    maximumHeight: 360

                    icon: "preferences-system"
                    flags: Qt.Dialog | Qt.WindowCloseButtonHint
                    modality: Qt.ApplicationModal

                    property real currentStackIndex: 0

                    property bool showSuccess: false

                    onClosing: function(close) {
                        close.accepted = true
                    }

                    ColumnLayout {
                        visible: !developDlg.showSuccess
                        Label {
                            visible: !developDlg.showSuccess
                            font: D.DTK.fontManager.t5
                            Layout.alignment: Qt.AlignHCenter
                            text: qsTr("Root Access")
                        }

                        // 单选按钮用于切换页面
                        RowLayout {
                            visible: !developDlg.showSuccess
                            spacing: 20
                            Layout.alignment: Qt.AlignHCenter
                            RadioButton {
                                id: radio1
                                text: qsTr("Online")
                                font: D.DTK.fontManager.t6
                                checked: true
                                onClicked: {
                                    if (developDlg.currentStackIndex === 0) {
                                        return
                                    }
                                    developDlg.currentStackIndex = 0;
                                    stackView.replace(page1Component);
                                }
                            }

                            RadioButton {
                                id: radio2
                                text: qsTr("Offline")
                                font: D.DTK.fontManager.t6
                                onClicked: {
                                    if (developDlg.currentStackIndex === 1) {
                                        return
                                    }

                                    developDlg.currentStackIndex = 1;
                                    stackView.replace(page2Component);
                                }
                            }
                        }

                        // StackView 用于显示不同页面
                        StackView {
                            visible: !developDlg.showSuccess
                            id: stackView
                            width: 340
                            height: 180
                            initialItem: page1Component
                        }

                        D.RecommandButton {
                            visible: !developDlg.showSuccess
                            id: confirmBtn
                            text: developDlg.currentStackIndex === 1 ? qsTr("Import Certificate") : (dccData.mode().isLogin ? qsTr("Request Root Access") : qsTr("Login UOS ID"))
                            font: D.DTK.fontManager.t7

                            Layout.fillWidth: true
                            Layout.bottomMargin: 10

                            onClicked: {
                                if (developDlg.currentStackIndex === 1) {
                                    fileDlg.open()
                                    return
                                }

                                if (dccData.mode().isLogin) {
                                    dccData.work().setEnableDeveloperMode(true)
                                    developDlg.close()
                                } else {
                                    dccData.work().login()
                                }
                            }
                        }
                        FileDialog {
                            id: exportFileDlg
                            title: qsTr("Select file")
                            nameFilters: "Text files (*.json)"
                            fileMode: FileDialog.SaveFile
                            folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
                            onAccepted: {
                                dccData.work().exportMessage(exportFileDlg.file)
                                developDlg.close()
                            }
                        }

                        FileDialog {
                            id: fileDlg
                            title: qsTr("Select file")
                            folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
                            onAccepted: {
                                dccData.work().importCertificate(fileDlg.file)

                            }
                        }

                        // 页面1
                        Component {
                            id: page1Component
                            ColumnLayout {
                                Rectangle {
                                    color: "transparent"
                                    width: parent.width
                                    height: 128
                                    Image {
                                        id: tikc
                                        source: dccData.mode().isLogin ? "common_tick" : "develop_bind"
                                        width: 128
                                        height: 128
                                        z: 2
                                        anchors.centerIn: parent
                                    }

                                    Image {
                                        id: ok
                                        visible: dccData.mode().isLogin
                                        source: "common_ok"
                                        z: 1
                                        anchors.centerIn: parent
                                        width: 128
                                        height: 128
                                    }
                                    Image {
                                        id: shadow
                                        visible: dccData.mode().isLogin
                                        source: "common_inner_shadow"
                                        z: 0
                                        anchors.centerIn: parent
                                        width: 128
                                        height: 128
                                    }
                                }

                                Item {
                                    Layout.fillHeight: true
                                }
                                Label {
                                    wrapMode: Text.WordWrap

                                    horizontalAlignment: Qt.AlignHCenter
                                    Layout.fillWidth: true
                                    font.pixelSize: 12
                                    Layout.bottomMargin: 10
                                    text: dccData.mode().isLogin ? qsTr("Your UOS ID has been logged in, click to enter developer mode") : qsTr("Please sign in to your UOS ID first and continue")
                                }
                            }
                        }

                        // 页面2
                        Component {
                            id: page2Component
                            ColumnLayout {
                                id: offlinePanel
                                spacing: 1
                                property int radius: 6
                                
                                Control {
                                    Layout.fillWidth: true
                                    height: 42
                                    padding: 0
                                    property bool checked: false
                                    property bool cascadeSelected: false
                                    property int corners: D.RoundRectangle.TopCorner
                                    background: DccItemBackground {
                                        backgroundType: DccObject.Normal
                                        radius: offlinePanel.radius
                                        shadowVisible: false
                                        backgroundColor: D.Palette {
                                            normal: Qt.rgba(0, 0, 0, 0.05)
                                            normalDark: Qt.rgba(247, 247, 247, 0.05)
                                        }
                                    }
                                    contentItem: RowLayout {
                                        Layout.leftMargin: 10
                                        Layout.rightMargin: 10
                                        Label {
                                            Layout.fillWidth: true
                                            Layout.leftMargin: 6
                                            text: qsTr("1.Export PC Info")
                                            font: D.DTK.fontManager.t6
                                            elide: Text.ElideMiddle
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        Button {
                                            id: exportBtn
                                            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                            Layout.rightMargin: 6
                                            implicitWidth: 60
                                            implicitHeight: 30
                                            text: qsTr("Export")
                                            onClicked: exportFileDlg.open()
                                        }
                                    }
                                }

                                Control {
                                    Layout.fillWidth: true
                                    padding: 0
                                    implicitHeight: 55
                                    property bool checked: false
                                    property bool cascadeSelected: false
                                    property int corners: 0
                                    background: DccItemBackground {
                                        backgroundType: DccObject.Normal
                                        radius: offlinePanel.radius
                                        shadowVisible: false
                                        backgroundColor: D.Palette {
                                            normal: Qt.rgba(0, 0, 0, 0.05)
                                            normalDark: Qt.rgba(247, 247, 247, 0.05)
                                        }
                                    }
                                    contentItem: ColumnLayout {
                                        Label {
                                            id: offlineLinkLabel
                                            Layout.fillWidth: true
                                            Layout.leftMargin: 6
                                            Layout.rightMargin: 10
                                            text: qsTr("2.please go to %1 to Download offline certificate.").arg("<a style='text-decoration: none;' href=\"http://www.chinauos.com/developMode\">http：//www.chinauos.com/developMode</a>")
                                            textFormat: Text.RichText
                                            wrapMode: Text.WordWrap
                                            font: D.DTK.fontManager.t6
                                            onLinkActivated: function(link) {
                                                Qt.openUrlExternally(link)
                                            }
                                            
                                            MouseArea {
                                                anchors.fill: parent
                                                acceptedButtons: Qt.LeftButton
                                                hoverEnabled: true
                                                cursorShape: offlineLinkLabel.linkAt(mouseX, mouseY) ? Qt.PointingHandCursor : Qt.ArrowCursor
                                                onClicked: function(mouse) {
                                                    var link = offlineLinkLabel.linkAt(mouse.x, mouse.y)
                                                    if (link) {
                                                        Qt.openUrlExternally(link)
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }

                                Control {
                                    Layout.fillWidth: true
                                    height: 40
                                    padding: 0
                                    property bool checked: false
                                    property bool cascadeSelected: false
                                    property int corners: D.RoundRectangle.BottomCorner
                                    background: DccItemBackground {
                                        backgroundType: DccObject.Normal
                                        radius: offlinePanel.radius
                                        shadowVisible: false
                                        backgroundColor: D.Palette {
                                            normal: Qt.rgba(0, 0, 0, 0.05)
                                            normalDark: Qt.rgba(247, 247, 247, 0.05)
                                        }
                                    }
                                    contentItem: RowLayout {
                                        Layout.leftMargin: 10
                                        Layout.rightMargin: 10
                                        Label {
                                            Layout.fillWidth: true
                                            Layout.leftMargin: 6
                                            text: qsTr("3.Import Certificate")
                                            font: D.DTK.fontManager.t6
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                    }
                                }

                                Item {
                                    Layout.fillHeight: true
                                }
                            }
                        }
                    }

                        ColumnLayout {
                            id: successView
                            visible: developDlg.showSuccess
                            anchors.fill: parent
                            spacing: 10
                            Label {
                                font: D.DTK.fontManager.t5
                                Layout.alignment: Qt.AlignHCenter
                                Layout.bottomMargin: 24
                                text: qsTr("Root Access")
                            }
                            Rectangle {
                                color: "transparent"
                                width: 128
                                height: 128
                                Layout.alignment: Qt.AlignHCenter
                                Image { source: "common_tick"; width: 128; height: 128; z: 2; anchors.centerIn: parent }
                                Image { source: "common_ok"; z: 1; anchors.centerIn: parent; width: 128; height: 128 }
                                Image { source: "common_inner_shadow"; z: 0; anchors.centerIn: parent; width: 128; height: 128 }
                            }
                            Label {
                                Layout.alignment: Qt.AlignHCenter
                                horizontalAlignment: Qt.AlignHCenter
                                text: qsTr("You have entered developer mode")
                                font: D.DTK.fontManager.t8
                            }
                            D.RecommandButton {
                                Layout.fillWidth: true
                                Layout.alignment: Qt.AlignHCenter
                                Layout.bottomMargin: 6
                                text: qsTr("OK")
                                font: D.DTK.fontManager.t7
                                onClicked: developDlg.close()
                            }
                        }
                }
                Connections {
                    target: dccData.mode()
                    function onDeveloperModeStateChanged(enable) {
                        if (enable && developDlg.currentStackIndex === 1) {
                            developDlg.showSuccess = true
                        }
                    }
                    function onIsDeveloperModeChanged() {
                        if (dccData.mode().isDeveloperMode && developDlg.currentStackIndex === 1) {
                            developDlg.showSuccess = true
                        }
                    }
                }
            }
        }


        DccObject {
            name: "developTips"
            parentName: "developerModeSetting"
            weight: 40
            pageType: DccObject.Item
            page: Label {
                id: securityCenterLinkLabel
                height: 30
                leftPadding: 15
                topPadding: 5
                bottomPadding: 5
                color: D.DTK.themeType === D.ApplicationHelper.LightType ? "#64000000" : "#64FFFFFF"
                textFormat: Text.RichText
                text: dccData.work().isSecurityCenterInstalled() ?
                    qsTr("To install and run unsigned apps, please go to <a style='text-decoration: none;' href='Security Center'> Security Center </a> to change the settings.") :
                    qsTr("To install and run unsigned apps, please go to Security Center to change the settings.")
                font: D.DTK.fontManager.t10
                // 超链接点击事件
                onLinkActivated: function(url) {
                    if (dccData.work().isSecurityCenterInstalled()) {
                        console.log("点击的链接是: " + url)
                        dccData.work().jumpToSecurityCenter();
                    }
                }
                
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    hoverEnabled: true
                    cursorShape: securityCenterLinkLabel.linkAt(mouseX, mouseY) ? Qt.PointingHandCursor : Qt.ArrowCursor
                    onClicked: function(mouse) {
                        var link = securityCenterLinkLabel.linkAt(mouse.x, mouse.y)
                        if (link && dccData.work().isSecurityCenterInstalled()) {
                            dccData.work().jumpToSecurityCenter();
                        }
                    }
                }
            }
        }
    }

    DccObject {
        name: "developDebugTitle"
        parentName: "developerMode"
        displayName: qsTr("Development and debugging options")
        weight: 50
        pageType: DccObject.Item
        page: Label {
            topPadding: 5
            leftPadding: 15
            bottomPadding: -2
            text: dccObj.displayName
            font.pixelSize: D.DTK.fontManager.t5.pixelSize
            font.weight: 500
            color: D.DTK.themeType === D.ApplicationHelper.LightType ?
                    Qt.rgba(0, 0, 0, 1) : Qt.rgba(1, 1, 1, 1)
        }
    }

    DccObject {
        name: "developDebugGrp"
        parentName: "developerMode"
        weight: 60
        pageType: DccObject.Item

        page: DccGroupView {

            Layout.topMargin: 0
        }

        DccObject {
            name: "developDebug"
            parentName: "developDebugGrp"
            displayName: qsTr("System logging level")
            description: qsTr("Changing the options results in more detailed logging that may degrade system performance and/or take up more storage space.")
            weight: 10
            backgroundType: DccObject.Normal
            pageType: DccObject.Editor
            page:  Row{
                ComboBox {
                    id: debugLogCombo
                    model: [ qsTr("Off"), qsTr("Debug") ]
                    flat: true
                    font: D.DTK.fontManager.t8
                    currentIndex: dccData.mode().debugLogCurrentIndex
                    onCurrentIndexChanged: {
                        console.log("Selected index:", currentIndex)
                        if (currentIndex !== dccData.mode().debugLogCurrentIndex) {
                            dccData.work().setLogDebug(currentIndex)
                        }
                    }
                }
            }
        }

        DccObject {
            name: "developDebugTips"
            parentName: "developDebugGrp"
            weight: 20
            pageType: DccObject.Item
            page: Label {
                leftPadding: 15
                bottomPadding: 5
                topPadding: 5
                rightPadding: 10
                text: qsTr("Changing the option may take up to a minute to process, after receiving a successful setting prompt, please reboot the device to take effect.")
                font: D.DTK.fontManager.t10
                opacity: 0.5
                wrapMode: Text.WordWrap
            }
        }


    }
}
