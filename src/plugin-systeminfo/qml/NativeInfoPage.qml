// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import org.deepin.dtk 1.0
import org.deepin.dcc 1.0
DccObject {
    id: root11
    DccObject {
        name: "systemLogo"
        weight: 10
        parentName: "systemInfo"
        pageType: DccObject.Item
        backgroundType: DccObject.Normal
        visible: !dccData.systemInfoMode().showDetail
        page: RowLayout {
            Image {
                // source: "qrc:/icons/deepin/builtin/icons/dcc_nav_systeminfo_84px.svg"
                source: "file://" + DTK.deepinDistributionOrgLogo
            }

            ColumnLayout {
                Label {
                    font.bold: true
                    font.pixelSize: 22
                    horizontalAlignment: Text.AlignLeft
                    text: qsTr("UOS")
                }

                Label {
                    font: DTK.fontManager.t6
                    horizontalAlignment: Text.AlignLeft
                    text: dccData.systemInfoMode().systemCopyright
                }
            }
        }
    }

    DccObject {
        name: "systemDetailLogo"
        weight: 20
        parentName: "systemInfo"
        pageType: DccObject.Item
        backgroundType: DccObject.Normal
        visible: dccData.systemInfoMode().showDetail
        page: ColumnLayout{
            Layout.topMargin: 10
            Layout.bottomMargin: 10
            Image {
                Layout.topMargin: 10
                Layout.alignment: Qt.AlignHCenter
                source: "file://" + dccData.systemInfoMode().logoPath
            }
            Label {
                Layout.alignment: Qt.AlignHCenter
                font: DTK.fontManager.t6
                text: dccData.systemInfoMode().systemCopyright
                Layout.bottomMargin: 10
            }
        }
    }

    DccObject {
        name: "nativeInfoGrp"
        parentName: "systemInfo"
        weight: 40
        pageType: DccObject.Item
        page: ColumnLayout {
            DccGroupView {
                Layout.topMargin: 10
            }
        }
        DccObject {
            name: "productName"
            weight: 10
            parentName: "nativeInfoGrp"
            displayName: qsTr("Computer name") + ":"
            backgroundType: DccObject.Normal
            visible: dccData.systemInfoMode().showDetail
            pageType: DccObject.Editor
            page: RowLayout {

                Label {
                    id: hostNameLabel
                    Layout.alignment: Qt.AlignRight | Qt.AlignTop
                    text: dccData.systemInfoMode().hostName
                    font: DTK.fontManager.t6
                }

                IconButton {
                    id: editBtn
                    icon.name: "qrc:/icons/deepin/builtin/texts/dcc_edit_12px.svg"
                    icon.width: 12
                    icon.height: 12
                    implicitWidth: 20
                    implicitHeight: 20
                    background: Rectangle {
                        color: "transparent" // 背景透明
                        border.color: "transparent" // 边框透明
                        border.width: 0
                    }
                    onClicked: {
                        editBtn.visible = false
                        hostNameLabel.visible = false
                        hostNameEdit.visible = true
                        hostNameEdit.text = dccData.systemInfoMode().hostName
                        hostNameEdit.selectAll()
                    }
                }

                LineEdit {
                    id: hostNameEdit
                    horizontalAlignment: Text.AlignHCenter
                    text: dccData.systemInfoMode().hostName
                    visible: false
                    showAlert: false
                    validator: RegularExpressionValidator {
                        regularExpression: /^[A-Za-z0-9-]{0,64}$/ // 允许的输入模式
                    }
                    onEditingFinished: {

                        if (hostNameEdit.text.length === 0) {
                            editBtn.visible = true
                            hostNameLabel.visible = true
                            hostNameEdit.visible = false
                            hostNameLabel.showAlert = false
                            return
                        }

                        if ((hostNameEdit.text.indexOf('-') === 0 || hostNameEdit.text.lastIndexOf('-') === hostNameEdit.text.length - 1) && hostNameEdit.text.length <= 63) {

                            hostNameEdit.showAlert = true
                            hostNameEdit.alertText = qsTr("It cannot start or end with dashes")
                            return
                        }

                        editBtn.visible = true
                        hostNameLabel.visible = true
                        hostNameEdit.visible = false
                        hostNameEdit.showAlert = false
                        dccData.systemInfoWork().onSetHostname(hostNameEdit.text)

                    }
                    Keys.onPressed: {
                        if (event.key === Qt.Key_Return) {
                            hostNameEdit.forceActiveFocus(false); // 结束编辑
                        }
                    }
                    onFocusChanged: {
                        console.log(" =============== hostNameEdit Focus " )
                    }
                }
            }
        }
        DccObject {
            name: "hostName"
            weight: 20
            parentName: "nativeInfoGrp"
            displayName: qsTr("OS Name") + ":"
            backgroundType: DccObject.Normal
            pageType: DccObject.Editor
            page: Label {
                Layout.alignment: Qt.AlignRight | Qt.AlignTop
                text: dccData.systemInfoMode().productName
                font: DTK.fontManager.t6
            }
        }
        DccObject {
            name: "version"
            weight: 30
            parentName: "nativeInfoGrp"
            pageType: DccObject.Editor
            displayName: qsTr("Version") + ":"
            page: Label {
                font: DTK.fontManager.t6
                horizontalAlignment: Text.AlignLeft
                text: dccData.systemInfoMode().versionNumber
            }
        }
        DccObject {
            name: "edition"
            weight: 40
            parentName: "nativeInfoGrp"
            pageType: DccObject.Editor
            displayName: qsTr("Edition") + ":"
            page: Label {
                font: DTK.fontManager.t6
                horizontalAlignment: Text.AlignLeft
                text: dccData.systemInfoMode().version
            }
        }
        DccObject {
            name: "type"
            weight: 50
            parentName: "nativeInfoGrp"
            pageType: DccObject.Editor
            displayName: qsTr("Type") + ":"
            page: Label {
                font: DTK.fontManager.t6
                horizontalAlignment: Text.AlignLeft
                text: dccData.systemInfoMode().type+ "-" + qsTr("bit")
            }
        }

        DccObject {
            name: "authorization"
            weight: 60
            parentName: "nativeInfoGrp"
            pageType: DccObject.Editor
            displayName: qsTr("Authorization") + ":"
            visible: dccData.systemInfoMode().showAuthorization()
            page: RowLayout {
                Label {
                    color: dccData.systemInfoMode().licenseStatusColor
                    font: DTK.fontManager.t6
                    horizontalAlignment: Text.AlignLeft
                    text: dccData.systemInfoMode().licenseStatusText
                }

                ToolButton {
                    text: dccData.systemInfoMode().licenseActionText
                    ColorSelector.family: Palette.CommonColor
                    font: DTK.fontManager.t6
                    flat: false
                    visible: dccData.systemInfoMode().showDetail
                    opacity: 0.7
                    onClicked: {
                        dccData.systemInfoWork().showActivatorDialog()
                    }
                }
            }
        }

        DccObject {
            name: "systemInstallationTime"
            weight: 70
            visible: dccData.systemInfoMode().showAuthorization()
            parentName: "nativeInfoGrp"
            pageType: DccObject.Editor
            displayName: qsTr("System installation time") + ":"
            page: Label {
                font: DTK.fontManager.t6
                horizontalAlignment: Text.AlignLeft
                text: dccData.systemInfoMode().systemInstallationDate
            }
        }

        DccObject {
            name: "kernel"
            weight: 80
            parentName: "nativeInfoGrp"
            pageType: DccObject.Editor
            displayName: qsTr("Kernel") + ":"
            page: Label {
                font: DTK.fontManager.t6
                horizontalAlignment: Text.AlignLeft
                text: dccData.systemInfoMode().kernel
            }
        }

        DccObject {
            name: "graphicsPlatform"
            weight: 90
            parentName: "nativeInfoGrp"
            pageType: DccObject.Editor
            displayName: qsTr("Graphics Platform") + ":"
            page: Label {
                font: DTK.fontManager.t6
                horizontalAlignment: Text.AlignLeft
                text: dccData.systemInfoMode().graphicsPlatform
            }
        }

        DccObject {
            name: "processor"
            weight: 100
            parentName: "nativeInfoGrp"
            pageType: DccObject.Editor
            displayName: qsTr("Processor") + ":"
            page: Label {
                font: DTK.fontManager.t6
                horizontalAlignment: Text.AlignLeft
                text: dccData.systemInfoMode().processor
            }
        }

        DccObject {
            name: "memory"
            weight: 100
            parentName: "nativeInfoGrp"
            pageType: DccObject.Editor
            displayName: qsTr("Memory") + ":"
            page: Label {
                font: DTK.fontManager.t6
                horizontalAlignment: Text.AlignLeft
                text: dccData.systemInfoMode().memory
            }
        }
    }

    DccObject {
        name: "detailBtn"
        weight: 60
        parentName: "systemInfo"
        pageType: DccObject.Item
        visible: !dccData.systemInfoMode().showDetail
        page: RowLayout {

            Button {
                Layout.topMargin: 10
                implicitWidth: 250
                implicitHeight: 30
                Layout.alignment: Qt.AlignHCenter
                text: "显示详细信息"
                font: DTK.fontManager.t6
                opacity: 0.7
                onClicked: {
                    dccData.systemInfoWork().showDetail()
                }
            }
        }
    }
}
