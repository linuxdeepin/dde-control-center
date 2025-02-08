// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
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
                max: 10
            }
        ]
        property var listDatas: [dccData.model.facesList, dccData.model.thumbsList, dccData.faceModel]
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

                        function requestRename(id, newName) {
                            switch (itemRep.authType) {
                            case CharaMangerModel.Type_Face:
                                dccData.renameFace(id, newName);
                                break;
                            case CharaMangerModel.Type_Finger:
                                dccData.requestRenameFinger(id, newName);
                                break;
                            case CharaMangerModel.Type_Iris:
                                // dccData.startEnrollIris()
                                break;
                            }
                        }

                        function requestDelete(id) {
                            switch (itemRep.authType) {
                            case CharaMangerModel.Type_Face:
                                dccData.removeFace(id);
                                break;
                            case CharaMangerModel.Type_Finger:
                                dccData.requestRemoveFinger(id);
                                break;
                            case CharaMangerModel.Type_Iris:
                                // dccData.startEnrollIris()
                                break;
                            }
                        }

                        TextInput {
                            id: textInputItem
                            text: modelData
                            Layout.leftMargin: 10
                            Layout.preferredHeight: DS.Style.itemDelegate.height
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            focus: false
                            wrapMode: Text.NoWrap
                            readOnly: true
                            focusPolicy: Qt.NoFocus
                            color: palette.text
                            onEditingFinished: {
                                focus = false;
                                if (modelData !== textInputItem.text) {
                                    layout.requestRename(modelData, text);
                                }
                            }
                            onFocusChanged: {
                                if (!focus)
                                    readOnly = true;
                            }
                            Keys.onEnterPressed: {
                                focus = false;
                            }
                        }
                        Item {
                            Layout.fillWidth: true
                        }
                        D.ToolButton {
                            id: editButton
                            visible: hoverHandler.hovered
                            icon.name: "dcc_edit"
                            background: Rectangle {
                                color: "transparent"
                            }
                            onClicked: {
                                textInputItem.readOnly = false;
                                textInputItem.focus = true;
                            }
                        }
                        D.ToolButton {
                            id: deleteButton
                            visible: hoverHandler.hovered
                            icon.name: "dcc_delete"
                            background: Rectangle {
                                color: "transparent"
                            }
                            onClicked: {
                                layout.requestDelete(modelData);
                            }
                        }

                        HoverHandler {
                            id: hoverHandler
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
                        text: qsTr("Add a new ") + modelData.authTitle + "..."
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
                                dccData.startEnrollIris();
                                break;
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
                            sourceComponent: AddFaceinfoDialog {
                                onClosing: function (close) {
                                    addIrisDialogLoader.active = false;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
