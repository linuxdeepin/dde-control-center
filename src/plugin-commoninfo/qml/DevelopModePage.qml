// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.0
import org.deepin.dtk 1.0

import org.deepin.dcc 1.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import Qt.labs.platform


DccObject {
    DccObject {
        name: "developTitle"
        parentName: "developerMode"
        weight: 10
        hasBackground: false
        pageType: DccObject.Item
        page: Label {
            leftPadding: 15
            bottomPadding: 5
            text: qsTr("Root Access")
            font.bold: true
            color: "black"
            font.pointSize: 14
        }
    }

    DccObject {
        name: "developerModeSetting"
        parentName: "developerMode"
        weight: 20
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "developerModeStatus"
            parentName: "developerModeSetting"
            pageType: DccObject.Item
            hasBackground: true
            weight: 20
            page: RowLayout {
                width: parent.width
                height: 60
                Column {
                    height: parent.height
                    Layout.fillWidth: true

                    Label {
                        topPadding: 10
                        leftPadding: 15
                        bottomPadding: 5
                        text: qsTr("Request Root Access")
                        font.pointSize: 14
                        color:"#7A000000"
                    }

                    Label {
                        width: 500
                        height: 40
                        leftPadding: 15
                        bottomPadding: 10
                        wrapMode: Text.WordWrap
                        text: qsTr("After entering the developer mode, you can obtain root permissions, but it may also damage the system integrity, so please use it with caution.")
                        font.pointSize: 10
                        color:"#5A000000"
                    }
                }

                Label {
                    Layout.alignment: Qt.AlignRight
                    visible: dccData.mode().developerModeState || dccData.mode().isActivate
                    leftPadding: 15
                    bottomPadding: 5
                    text: qsTr("Allowed")
                    font.pointSize: 10
                }

                Button {
                    Layout.alignment: Qt.AlignRight
                    implicitWidth: 50
                    visible: !(dccData.mode().developerModeState || dccData.mode().isActivate)
                    text: qsTr("Enter")

                    onClicked: {
                        developDlg.show()
                    }
                }

                DialogWindow {
                    id: developDlg
                    width: 360
                    height: 350

                    icon: "preferences-system"
                    flags: Qt.Dialog | Qt.WindowCloseButtonHint
                    modality: Qt.ApplicationModal

                    property real currentStackIndex: 0

                    onClosing: function(close) {
                        close.accepted = true
                    }

                    ColumnLayout {
                        width: parent.width
                        spacing: 10
                        Label {
                            height: 20
                            font: DTK.fontManager.t5
                            Layout.alignment: Qt.AlignHCenter
                            text: qsTr("Root Access")
                        }

                        // 单选按钮用于切换页面
                        Row {
                            width: 340
                            spacing: 20
                            height: 20
                            Layout.alignment: Qt.AlignHCenter
                            RadioButton {
                                id: radio1
                                text: qsTr("Online")
                                font: DTK.fontManager.t6
                                checked: true
                                onClicked: {
                                    if (developDlg.currentStackIndex === 0) {
                                        return
                                    }
                                    developDlg.currentStackIndex = 0;
                                    stackView.replace(page1Component);
                                    confirmBtn.text = qsTr("Login UOS ID")
                                }
                            }

                            RadioButton {
                                id: radio2
                                text: qsTr("Offline")
                                font: DTK.fontManager.t6
                                onClicked: {
                                    if (developDlg.currentStackIndex === 1) {
                                        return
                                    }

                                    developDlg.currentStackIndex = 1;
                                    stackView.replace(page2Component);
                                    confirmBtn.text = qsTr("Import Certificate")
                                }
                            }
                        }

                        // StackView 用于显示不同页面
                        StackView {
                            id: stackView
                            width: 340
                            height: 160
                            initialItem: page1Component

                        }

                        RecommandButton {
                            id: confirmBtn
                            text: dccData.mode().isLogin ? qsTr("Request Root Access") : qsTr("Login UOS ID")
                            width: parent.width
                            font: DTK.fontManager.t7
                            height: 20

                            onClicked: {
                                if (developDlg.currentStackIndex === 1) {
                                    fileDlg.open()
                                    return
                                }

                                if (dccData.mode().isLogin) {
                                    dccData.work().setEnableDeveloperMode(true)
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
                            ColumnLayout{
                                Rectangle {
                                    color: "transparent"
                                    width: 340
                                    height: 128
                                    Image {
                                        id: tikc
                                        source: dccData.mode().isLogin ? "common_tick" : "qrc:/icons/deepin/builtin/icons/develop_bind.dci"
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

                                Label {
                                    height: 20
                                    font: DTK.fontManager.t8
                                    Layout.alignment: Qt.AlignHCenter
                                    text: dccData.mode().isLogin ? qsTr("Your UOS ID has been logged in, click to enter developer mode") : qsTr("Please sign in to your UOS ID first and continue")
                                }
                            }
                        }

                        // 页面2
                        Component {
                            id: page2Component

                            Column {
                                spacing: 2
                                Rectangle {
                                    width: 340
                                    height: 50
                                    color: "#1A000000"
                                    radius: 6
                                    RowLayout {
                                        height: parent.height
                                        width: parent.width
                                        Label {
                                            leftPadding: 10
                                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                            text: qsTr("1.Export PC Info")
                                        }

                                        Button {
                                            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                            implicitWidth: 40
                                            Layout.rightMargin: 10
                                            text: qsTr("Export")

                                            onClicked: {
                                                exportFileDlg.open()
                                            }
                                        }
                                    }
                                }

                                Rectangle {
                                    width: 340
                                    height: 50
                                    color: "#1A000000"
                                    radius: 6
                                    Label {
                                        anchors.centerIn: parent
                                        height: 30
                                        width: 340
                                        leftPadding: 10
                                        text: qsTr("2.please go to <a href=\"http://www.chinauos.com/developMode\">http：//www.chinauos.com/developMode</a> to Download offline certificate.")
                                        font.pointSize: 10
                                        wrapMode: Text.WordWrap
                                        color:"#5A000000"
                                        // 超链接点击事件
                                        onLinkActivated: function(url) {
                                            console.log("点击的链接是: " + url)
                                            Qt.openUrlExternally(url) // 使用默认浏览器打开链接
                                        }
                                    }
                                }

                                Rectangle {
                                    width: 340
                                    height: 40
                                    color: "#1A000000"
                                    radius: 6
                                    Label {
                                        height: parent.height
                                        verticalAlignment: Text.AlignVCenter
                                        leftPadding: 10
                                        text: qsTr("Import Certificate")
                                        font: DTK.fontManager.t6
                                        color:"#5A000000"
                                        // 超链接点击事件
                                        onLinkActivated: function(url) {
                                            Qt.openUrlExternally(url) // 使用默认浏览器打开链接
                                        }
                                    }
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
            hasBackground: false
            pageType: DccObject.Item
            page: Label {
                height: 30
                leftPadding: 15
                topPadding: 10
                bottomPadding: 5
                text: qsTr("To install and run unsigned apps, please go to <a href=\"Security Center\">Security Center</a> to change the settings.")
                font.pointSize: 10
                //opacity: 0.5
                color:"#5A000000"
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
        hasBackground: false
        pageType: DccObject.Item
        page: Label {
            leftPadding: 15
            bottomPadding: 5
            text: qsTr("Development and debugging options")
            font.bold: true
            color: "black"
            font.pointSize: 14
        }
    }

    DccObject {
        name: "developDebug"
        parentName: "developerMode"
        displayName: qsTr("System logging level")
        description: qsTr("Changing the options results in more detailed logging that may degrade system performance and/or take up more storage space.")
        weight: 60
        hasBackground: true
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
                    dccData.work().setLogDebug(currentIndex)
                }
            }
        }
    }
}
