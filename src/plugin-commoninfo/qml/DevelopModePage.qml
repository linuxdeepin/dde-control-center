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
        weight: 10
        pageType: DccObject.Item
        page: Label {
            leftPadding: 15
            bottomPadding: 5
            text: qsTr("Root Access")
            font.bold: true
            font.pixelSize: 14
        }
    }

    DccObject {
        name: "developerModeSetting"
        parentName: "developerMode"
        weight: 20
        pageType: DccObject.Item
        page: DccGroupView {

            Layout.topMargin: 10
        }

        DccObject {
            name: "developerModeStatus"
            parentName: "developerModeSetting"
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
                        text: qsTr("Request Root Access")
                        font.pixelSize: 16
                    }

                    Label {
                        horizontalAlignment: Text.AlignLeft
                        wrapMode: Text.WordWrap
                        text: qsTr("After entering the developer mode, you can obtain root permissions, but it may also damage the system integrity, so please use it with caution.")
                        font.pixelSize: 12
                        opacity: 0.7

                        Layout.fillWidth: true
                    }
                }

                Label {
                    Layout.alignment: Qt.AlignRight
                    Layout.rightMargin: 10
                    visible: dccData.mode().developerModeState || dccData.mode().isDeveloperMode
                    text: qsTr("Allowed")
                    font.pixelSize: 12
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

                    onClosing: function(close) {
                        close.accepted = true
                    }

                    ColumnLayout {
                        Label {
                            font: D.DTK.fontManager.t5
                            Layout.alignment: Qt.AlignHCenter
                            text: qsTr("Root Access")
                        }

                        // 单选按钮用于切换页面
                        RowLayout {
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
                            id: stackView
                            width: 340
                            height: 180
                            initialItem: page1Component
                        }

                        D.RecommandButton {
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
                                spacing: 2
                                Rectangle {
                                    width: parent.width
                                    height: 40
                                    color: D.DTK.themeType === D.ApplicationHelper.LightType ?
                                        Qt.rgba(0, 0, 0, 0.05) : Qt.rgba(1, 1, 1, 0.05)
                                    radius: 6

                                    Layout.fillWidth: true

                                    RowLayout {
                                        height: parent.height
                                        width: parent.width

                                        Label {
                                            leftPadding: 10
                                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                            text: qsTr("1.Export PC Info")
                                            Layout.fillWidth: true
                                        }

                                        Button {
                                            id: exportBtn
                                            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                            implicitWidth: 40
                                            implicitHeight: 30
                                            Layout.rightMargin: 10
                                            text: qsTr("Export")

                                            onClicked: {
                                                exportFileDlg.open()
                                            }
                                        }
                                    }
                                }

                                Label {
                                    leftPadding: 10
                                    text: qsTr("2.please go to <a href=\"http://www.chinauos.com/developMode\">http：//www.chinauos.com/developMode</a> to Download offline certificate.")
                                    wrapMode: Text.WordWrap
                                    topPadding: 5
                                    bottomPadding: 5
                                    // 超链接点击事件
                                    onLinkActivated: function (url) {
                                        console.log("点击的链接是: " + url)
                                        Qt.openUrlExternally(url) // 使用默认浏览器打开链接
                                    }

                                    Layout.fillWidth: true

                                    background: Rectangle {
                                        color: D.DTK.themeType === D.ApplicationHelper.LightType ?
                                            Qt.rgba(0, 0, 0, 0.05) : Qt.rgba(1, 1, 1, 0.05)
                                        radius: 6
                                    }
                                }

                                Label {
                                    verticalAlignment: Text.AlignVCenter
                                    leftPadding: 10
                                    topPadding: 5
                                    bottomPadding: 5
                                    text: qsTr("3.Import Certificate")
                                    font: D.DTK.fontManager.t6
                                    // 超链接点击事件
                                    onLinkActivated: function (url) {
                                        Qt.openUrlExternally(url) // 使用默认浏览器打开链接
                                    }

                                    Layout.fillWidth: true

                                    background: Rectangle {
                                        color: D.DTK.themeType === D.ApplicationHelper.LightType ?
                                            Qt.rgba(0, 0, 0, 0.05) : Qt.rgba(1, 1, 1, 0.05)
                                        radius: 6 // 可选：设置圆角
                                    }
                                }

                                Item {
                                    Layout.fillHeight: true
                                }
                            }
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
                height: 30
                leftPadding: 15
                topPadding: 10
                bottomPadding: 5
                text: qsTr("To install and run unsigned apps, please go to <a href=\"Security Center\">Security Center</a> to change the settings.")
                font.pixelSize: 12
                opacity: 0.7
               // color: "transparent"
               // color:"#5A000000"
                // 超链接点击事件
                onLinkActivated: function(url) {
                    console.log("点击的链接是: " + url)
                    dccData.work().jumpToSecurityCenter();
                }
            }
        }
    }

    DccObject {
        name: "developDebugTitle"
        parentName: "developerMode"
        weight: 50
        pageType: DccObject.Item
        page: Label {
            leftPadding: 15
            bottomPadding: 5
            text: qsTr("Development and debugging options")
            font.bold: true
            color: "black"
            font.pixelSize: 16
        }
    }

    DccObject {
        name: "developDebugGrp"
        parentName: "developerMode"
        weight: 60
        pageType: DccObject.Item

        page: DccGroupView {

            Layout.topMargin: 10
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
                    width: 180
                    model: [ qsTr("Off"), qsTr("Debug") ]
                    flat: true
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
                font.pixelSize: 12
                opacity: 0.7
                wrapMode: Text.WordWrap
            }
        }


    }
}
