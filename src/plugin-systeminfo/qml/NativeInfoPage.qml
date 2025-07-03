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
import org.deepin.dtk.style 1.0 as DS
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
            Image {
                Layout.topMargin: 25
                Layout.alignment: Qt.AlignHCenter
                source: "file://" + dccData.systemInfoMode().logoPath
            }
            Label {
                Layout.alignment: Qt.AlignHCenter
                text: dccData.systemInfoMode().systemCopyright
                Layout.bottomMargin: 25
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
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    text: dccData.systemInfoMode().hostName
                    ToolTip {
                        text: hostNameLabel.text
                        delay: 500
                        visible: hostNameArea.containsMouse
                    }

                    MouseArea {
                        id: hostNameArea
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }

                ActionButton {
                    id: editBtn
                    icon.name: "dcc_systemInfo_edit"
                    icon.width: 16
                    icon.height: 16
                    implicitWidth: 30
                    implicitHeight: 30
                    flat: !hovered
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

                    palette.windowText: ColorSelector.textColor
                    property Palette textColor: parent.textColor

                    background: Rectangle {
                        property Palette pressedColor: Palette {
                            normal: Qt.rgba(0, 0, 0, 0.2)
                            normalDark: Qt.rgba(0, 0, 0, 0.15)
                        }
                        property Palette hoveredColor: Palette {
                            normal: Qt.rgba(0, 0, 0, 0.1)
                            normalDark: Qt.rgba(1, 1, 1, 0.1)
                        }
                        radius: DS.Style.control.radius
                        color: parent.pressed ? ColorSelector.pressedColor : (parent.hovered ? ColorSelector.hoveredColor : "transparent")

                        border {
                            color: parent.palette.highlight
                            width: parent.visualFocus ? DS.Style.control.focusBorderWidth : 0
                        }
                    }
                    
                    onClicked: {
                        editBtn.visible = false
                        hostNameLabel.visible = false
                        hostNameEdit.visible = true
                        hostNameEdit.text = dccData.systemInfoMode().hostName
                        hostNameEdit.selectAll()
                        hostNameEdit.forceActiveFocus()
                    }
                }

                LineEdit {
                    id: hostNameEdit
                    horizontalAlignment: TextInput.AlignLeft
                    text: dccData.systemInfoMode().hostName
                    visible: false
                    showAlert: false
                    
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton
                        onClicked: function(mouse) {
                            mouse.accepted = true
                        }
                    }
                    
                    onVisibleChanged: {
                        if (!visible && showAlert) {
                            showAlert = false
                        }
                    }
                    
                    onActiveFocusChanged: {
                        if (!activeFocus && showAlert) {
                            showAlert = false
                        }
                    }
                    
                    onTextChanged: {
                        if (showAlert)
                            showAlert = false
                            
                        if (text.length > 63) {
                            var cursorPos = cursorPosition
                            text = text.slice(0, 63)
                            cursorPosition = Math.min(cursorPos, text.length)
                            showAlert = true
                            alertText = qsTr("1~63 characters please")
                            dccData.systemInfoWork().playSystemSound(14)
                            return
                        }
                        
                        if (!/^[A-Za-z0-9-]{0,63}$/.test(text)) {
                            var cursorPos = cursorPosition
                            var filteredText = text.replace(/[^A-Za-z0-9-]/g, "")
                            
                            filteredText = filteredText.slice(0, 63)
                            
                            if (filteredText !== text) {
                                text = filteredText
                                cursorPosition = Math.min(cursorPos, text.length)
                                dccData.systemInfoWork().playSystemSound(14)
                            }
                        }
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
                    
                    Keys.onPressed: function(event) {
                        if (event.key === Qt.Key_Return) {
                            hostNameEdit.forceActiveFocus(false);
                        }
                        else if ((event.modifiers & Qt.ControlModifier) && 
                                (event.key === Qt.Key_C || event.key === Qt.Key_V || event.key === Qt.Key_X)) {
                            event.accepted = true
                        }
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
            }
        }
        DccObject {
            name: "version"
            weight: 30
            parentName: "nativeInfoGrp"
            pageType: DccObject.Editor
            displayName: qsTr("Version") + ":"
            page: Label {
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
                horizontalAlignment: Text.AlignLeft
                text: dccData.systemInfoMode().type + qsTr("bit")
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
                spacing: 8
                Label {
                    id: jihuo
                    color: dccData.systemInfoMode().licenseStatusColor
                    horizontalAlignment: Text.AlignLeft
                    text: dccData.systemInfoMode().licenseStatusText
                }

                Button {
                    text: dccData.systemInfoMode().licenseActionText
                    ColorSelector.family: Palette.CommonColor
                    implicitHeight: 30
                    implicitWidth: 50
                    flat: false
                    visible: dccData.systemInfoMode().showDetail
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
                horizontalAlignment: Text.AlignLeft
                text: dccData.systemInfoMode().kernel
            }
        }

        DccObject {
            name: "graphicsPlatform"
            weight: 90
            parentName: "nativeInfoGrp"
            pageType: DccObject.Editor
            visible: dccData.systemInfoMode().showGraphicsPlatform()
            displayName: qsTr("Graphics Platform") + ":"
            page: Label {
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
