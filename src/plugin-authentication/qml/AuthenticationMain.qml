// SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc.account.biometric 1.0

DccObject {
    id: root
    property var objParentName: "authentication"

    DccTitleObject {
        name: "BiometricAuthenticationTitle"
        parentName: objParentName
        weight: 10
        displayName: qsTr("Biometric Authentication")
    }

    DccRepeater {
        id: rep
        model: [
            {
                authTitle: qsTr("Face"),
                authDescription: qsTr("Up to 5 facial data can be entered"),
                authIcon: "user_biometric_face",
                authType: CharaMangerModel.Type_Face,
                available: dccData.model.faceDriverVaild,
                max: 5
            },
            {
                authTitle: qsTr("Fingerprint"),
                authDescription: qsTr("Identifying user identity through scanning fingerprints"),
                authIcon: "user_biometric_fingerprint",
                authType: CharaMangerModel.Type_Finger,
                available: dccData.model.fingerDriverVaild,
                max: 10
            },
            {
                authTitle: qsTr("Iris"),
                authDescription: qsTr("Identity recognition through iris scanning"),
                authIcon: "user_biometric_iris",
                authType: CharaMangerModel.Type_Iris,
                available: dccData.model.irisDriverVaild,
                max: 5
            }
        ]
        property var listDatas: [dccData.model.facesList, dccData.model.thumbsList, dccData.model.irisList]
        delegate: DccObject {
            id: authenticationGroupView
            name: "authenticationGroupView"
            parentName: root.objParentName
            weight: 20 + index
            pageType: DccObject.Item
            visible: modelData.available
            page: DccGroupView {}

            DccObject {
                id: repeaterHeaderView
                name: "repeaterHeaderView"
                parentName: root.objParentName + "/authenticationGroupView"
                description: modelData.authDescription
                displayName: modelData.authTitle
                icon: modelData.authIcon
                backgroundType: DccObject.Normal
                property bool listVisible: false
                property real iconSize: 30
                weight: 20
                pageType: DccObject.Editor
                page: RowLayout {
                    Control {
                        id: control
                        contentItem: D.DciIcon {
                            Layout.rightMargin: 5
                            name: "arrow_ordinary_up"
                            sourceSize: Qt.size(12, 12)
                            theme: D.DTK.themeType
                            palette: D.DTK.makeIconPalette(control.palette)

                            rotation: repeaterHeaderView.listVisible ? 0 : 180

                            Behavior on rotation {
                                NumberAnimation {
                                    duration: 100
                                }
                            }
                        }
                    }
                }

                Connections {
                    target: repeaterHeaderView.parentItem
                    function onClicked() {
                        repeaterHeaderView.listVisible = !repeaterHeaderView.listVisible;
                    }
                }
            }
            DccRepeater {
                id: itemRep
                property var authType: modelData.authType
                model: repeaterHeaderView.listVisible ? rep.listDatas[index] : []
                delegate: DccObject {
                    name: modelData
                    parentName: authenticationGroupView.name
                    displayName: modelData
                    pageType: DccObject.Item
                    weight: 30 + index
                    page: RowLayout {
                        id: layout
                        property bool showActions: false

                        function requestRename(id, newName) {
                            switch (itemRep.authType) {
                            case CharaMangerModel.Type_Face:
                                dccData.faceController.rename(id, newName);
                                break;
                            case CharaMangerModel.Type_Finger:
                                dccData.fingerprintController.rename(id, newName);
                                break;
                            case CharaMangerModel.Type_Iris:
                                dccData.irisController.rename(id, newName);
                                break;
                            }
                        }

                        function requestDelete(id) {
                            switch (itemRep.authType) {
                            case CharaMangerModel.Type_Face:
                                dccData.faceController.remove(id);
                                break;
                            case CharaMangerModel.Type_Finger:
                                dccData.fingerprintController.remove(id);
                                break;
                            case CharaMangerModel.Type_Iris:
                                dccData.irisController.remove(id);
                                break;
                            }
                        }

                        D.LineEdit {
                            id: nameEditor
                            property int maxLength: 15
                            Layout.leftMargin: 10
                            Layout.rightMargin: 10
                            Layout.fillWidth: true
                            implicitHeight: 30
                            Layout.topMargin: 5
                            Layout.bottomMargin: 5
                            horizontalAlignment: TextInput.AlignLeft
                            text: modelData
                            readOnly: true
                            clearButton.active: false

                            background: D.EditPanel {
                                id: nameEditPanel
                                control: nameEditor
                                showBorder: nameEditor.activeFocus && !nameEditor.readOnly
                                alertDuration: 3000
                                backgroundColor: D.Palette {
                                    normal: Qt.rgba(1, 1, 1, 0)
                                    normalDark: Qt.rgba(1, 1, 1, 0)
                                }
                            }

                            onTextEdited: {
                                if (nameEditPanel.showAlert) {
                                    nameEditPanel.showAlert = false
                                }
                                // 实时检测：过滤非法字符并限制长度
                                var filteredText = text;
                                // 过滤非法字符（只允许字母、数字、中文、下划线）
                                filteredText = filteredText.replace(/[^A-Za-z0-9\u4e00-\u9fa5_]/g, "");

                                // 检查是否超长
                                if (filteredText.length > maxLength) {
                                    nameEditPanel.showAlert = true
                                    nameEditPanel.alertText = qsTr("No more than 15 characters")
                                    filteredText = filteredText.slice(0, maxLength);
                                }

                                if (text !== filteredText) {
                                    text = filteredText;
                                }
                            }

                            onEditingFinished: {
                                if (readOnly) {
                                    return
                                }

                                if (nameEditPanel.showAlert) {
                                    nameEditPanel.showAlert = false
                                }

                                if (!checkInputInvalid()) {
                                    text = modelData;
                                    nameEditor.text = Qt.binding(function() {
                                        return modelData
                                    })
                                    return;
                                }
                                readOnly = true;
                                if (modelData !== nameEditor.text) {
                                    layout.requestRename(modelData, nameEditor.text);
                                }
                                nameEditor.text = Qt.binding(function() {
                                    return modelData
                                })
                            }

                            function checkInputInvalid() {
                                var reg = /^[A-Za-z0-9\u4e00-\u9fa5_]+$/;
                                var isValid = text.length === 0 || reg.test(nameEditor.text);
                                var isOverLength = nameEditor.text.length > maxLength;
                                var isEmpty = nameEditor.text.length === 0;

                                var nameList = [];
                                switch (itemRep.authType) {
                                case CharaMangerModel.Type_Face:
                                    nameList = dccData.model.facesList;
                                    break;
                                case CharaMangerModel.Type_Finger:
                                    nameList = dccData.model.thumbsList;
                                    break;
                                case CharaMangerModel.Type_Iris:
                                    nameList = dccData.model.irisList;
                                    break;
                                }

                                var isDuplicate = nameList.includes(nameEditor.text) &&
                                                  nameEditor.text !== modelData;

                                if (isEmpty) {
                                    nameEditPanel.showAlert = true
                                    nameEditPanel.alertText = qsTr("The name cannot be empty")
                                } else if (!isValid && isOverLength) {
                                    nameEditPanel.showAlert = true
                                    nameEditPanel.alertText = qsTr("Use letters, numbers and underscores only, and no more than 15 characters")
                                } else if (!isValid) {
                                    nameEditPanel.showAlert = true
                                    nameEditPanel.alertText = qsTr("Use letters, numbers and underscores only")
                                } else if (isOverLength) {
                                    nameEditPanel.showAlert = true
                                    nameEditPanel.alertText = qsTr("No more than 15 characters")
                                } else if (isDuplicate) {
                                    nameEditPanel.showAlert = true
                                    nameEditPanel.alertText = qsTr("This name already exists")
                                } else {
                                    return true;
                                }
                                return false;
                            }
                        }

                        RowLayout {
                            id: actionButtons
                            Layout.preferredWidth: 60
                            Layout.preferredHeight: 30
                            Layout.fillWidth: false
                            Layout.topMargin: 5
                            Layout.bottomMargin: 5
                            spacing: 0

                            D.ActionButton {
                                id: editButton
                                visible: hoverHandler.hovered || layout.showActions
                                implicitWidth: 30
                                implicitHeight: 30
                                icon.name: "dcc_edit"
                                icon.width: DS.Style.edit.actionIconSize
                                icon.height: DS.Style.edit.actionIconSize
                                background: Rectangle {
                                    property D.Palette pressedColor: D.Palette {
                                        normal: Qt.rgba(0, 0, 0, 0.2)
                                        normalDark: Qt.rgba(1, 1, 1, 0.25)
                                    }
                                    property D.Palette hoveredColor: D.Palette {
                                        normal: Qt.rgba(0, 0, 0, 0.1)
                                        normalDark: Qt.rgba(1, 1, 1, 0.1)
                                    }
                                    radius: DS.Style.control.radius
                                    color: parent.pressed ? D.ColorSelector.pressedColor : (parent.hovered ? D.ColorSelector.hoveredColor : "transparent")
                                    border {
                                        color: parent.palette.highlight
                                        width: parent.visualFocus ? DS.Style.control.focusBorderWidth : 0
                                    }
                                }
                                onClicked: {
                                    nameEditor.readOnly = false;
                                    nameEditor.forceActiveFocus();
                                }
                            }
                            D.ActionButton {
                                id: deleteButton
                                visible: hoverHandler.hovered || layout.showActions
                                implicitWidth: 30
                                implicitHeight: 30
                                icon.name: "dcc_delete"
                                icon.width: DS.Style.edit.actionIconSize
                                icon.height: DS.Style.edit.actionIconSize
                                background: Rectangle {
                                    property D.Palette pressedColor: D.Palette {
                                        normal: Qt.rgba(0, 0, 0, 0.2)
                                        normalDark: Qt.rgba(1, 1, 1, 0.25)
                                    }
                                    property D.Palette hoveredColor: D.Palette {
                                        normal: Qt.rgba(0, 0, 0, 0.1)
                                        normalDark: Qt.rgba(1, 1, 1, 0.1)
                                    }
                                    radius: DS.Style.control.radius
                                    color: parent.pressed ? D.ColorSelector.pressedColor : (parent.hovered ? D.ColorSelector.hoveredColor : "transparent")
                                    border {
                                        color: parent.palette.highlight
                                        width: parent.visualFocus ? DS.Style.control.focusBorderWidth : 0
                                    }
                                }
                                onClicked: {
                                    layout.requestDelete(modelData);
                                }
                            }
                        }

                        HoverHandler {
                            id: hoverHandler
                        }
                        
                        TapHandler {
                            id: touchHandler
                            acceptedDevices: PointerDevice.TouchScreen
                            onTapped: layout.showActions = !layout.showActions
                        }
                    }
                }
            }

            DccObject {
                name: root.objParentName + "FaceAuthentication"
                visible: repeaterHeaderView.listVisible && rep.listDatas[index].length < modelData.max
                parentName: authenticationGroupView.name
                pageType: DccObject.Item
                weight: 50
                page: RowLayout {
                    D.ToolButton {
                        implicitHeight: DS.Style.itemDelegate.height
                        Layout.leftMargin: 5
                        textColor: D.Palette {
                            normal {
                                common: D.DTK.makeColor(D.Color.Highlight)
                            }
                            normalDark: normal
                            hovered {
                                common: D.DTK.makeColor(D.Color.Highlight).lightness(+30)
                            }
                            hoveredDark: hovered
                        }
                        text: qsTr("Add a new %1 ...").arg(modelData.authTitle)
                        font: D.DTK.fontManager.t6
                        background: Item {}
                        onClicked: {
                            startEnrollByType(modelData.authType);
                        }

                        function startEnrollByType(type) {
                            switch (type) {
                            case CharaMangerModel.Type_Face:
                                addFaceDialogLoader.active = true;
                                addFaceDialogLoader.item.show();
                                break;
                            case CharaMangerModel.Type_Finger:
                                addFingerDialogLoader.active = true;
                                addFingerDialogLoader.item.show();
                                break;
                            case CharaMangerModel.Type_Iris:
                                addIrisDialogLoader.active = true;
                                addIrisDialogLoader.item.show();
                                break;
                            }
                        }
                    }
                }
            }

            Loader {
                id: addFaceDialogLoader
                active: false
                sourceComponent: AddFaceinfoDialog {
                    onClosing: function (close) {
                        addFaceDialogLoader.active = false;
                    }
                }
            }

            Loader {
                id: addFingerDialogLoader
                active: false
                sourceComponent: AddFingerDialog {
                    onClosing: function (close) {
                        addFingerDialogLoader.active = false;
                    }
                }
            }

            Loader {
                id: addIrisDialogLoader
                active: false
                sourceComponent: AddIrisDialog {
                    onClosing: function (close) {
                        addIrisDialogLoader.active = false;
                    }
                }
            }
        }
    }
}
