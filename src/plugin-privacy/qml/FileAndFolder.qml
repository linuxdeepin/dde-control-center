// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc.privacy 1.0

DccObject {
    DccObject {
        name: "filefolderViewGroup"
        parentName: "privacy/filefolder"
        weight: 100
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "title"
            weight: 1
            parentName: "privacy/filefolder/filefolderViewGroup"
            pageType: DccObject.Editor
            canSearch: false
            displayName: qsTr("Allow below apps to access these files and folders:")
        }

        DccRepeater {
            model: dccData.appsModel
            delegate: DccObject {
                id: privacyFolderItem
                name: "plugin" + model.name
                property real iconSize: 16
                property bool isExpanded: false
                property var dataModel: model
                parentName: "privacy/filefolder/filefolderViewGroup"
                weight: 10 + index * 10
                pageType: DccObject.Item
                visible: !model.noDisplay
                canSearch: false
                backgroundType: DccObject.ClickStyle

                onParentItemChanged: {
                    if (parentItem) {
                        parentItem.activeFocusOnTab = true
                    }
                }

                Connections {
                    target: parentItem
                    function onClicked() {
                        privacyFolderItem.isExpanded = !privacyFolderItem.isExpanded
                    }
                }

                DccRepeater {
                    id: rep
                    property var itemIndex: index
                    model: [
                        { name: qsTr("Documents"), premission: ApplicationItem.DocumentFoldersPermission},
                        { name: qsTr("Desktop"), premission: ApplicationItem.DesktopFoldersPermission},
                        { name: qsTr("Pictures"), premission: ApplicationItem.PictureFoldersPermission},
                        { name: qsTr("Videos"), premission: ApplicationItem.VideoFoldersPermission},
                        { name: qsTr("Music"), premission: ApplicationItem.MusicFoldersPermission},
                        { name: qsTr("Downloads"), premission: ApplicationItem.DownloadFoldersPermission}
                    ]

                    property var checkedStates: [
                        privacyFolderItem.dataModel.documentPermission,
                        privacyFolderItem.dataModel.desktopPermission,
                        privacyFolderItem.dataModel.picturePermission,
                        privacyFolderItem.dataModel.videoPermission,
                        privacyFolderItem.dataModel.musicPermission,
                        privacyFolderItem.dataModel.downloadPermission
                    ]

                    delegate: DccObject {
                        parentName: "privacy/filefolder/filefolderViewGroup"
                        weight: privacyFolderItem.weight + 1
                        canSearch: false
                        visible: privacyFolderItem.isExpanded
                        displayName: modelData.name
                        pageType: DccObject.Item
                        backgroundType: DccObject.Hover
                        page: RowLayout {
                            spacing: 2

                            Item {
                                Layout.preferredHeight: DS.Style.itemDelegate.height
                                Layout.preferredWidth: 34
                            }

                            DccLabel {
                                text: String('"%1" ').arg(modelData.name)
                                color: D.DTK.platformTheme.activeColor
                            }
                            DccLabel {
                                Layout.fillWidth: true
                                text: qsTr("folder")
                            }
                            D.Switch {
                                Layout.alignment: Qt.AlignRight
                                Layout.rightMargin: 10
                                checked: rep.checkedStates[index]

                                onCheckedChanged: {
                                    if (checked != rep.checkedStates[index]) {
                                        dccData.worker.setPremissionEnabled(rep.itemIndex, modelData.premission, checked)
                                    }
                                }
                            }
                        }
                    }
                }

                page: ColumnLayout {
                    RowLayout {
                        Layout.preferredHeight: DS.Style.itemDelegate.height
                        Layout.leftMargin: 10
                        Layout.rightMargin: 10
                        D.DciIcon {
                            sourceSize: Qt.size(DS.Style.itemDelegate.iconSize, DS.Style.itemDelegate.iconSize)
                            name: model.iconName
                            theme: D.DTK.themeType
                            palette: parent ? D.DTK.makeIconPalette(parent.palette) : D.DTK.makeIconPalette(D.DTK.palette)
                        }
                        DccLabel {
                            text: model.name
                        }
                        Item {
                            Layout.fillWidth: true
                        }
                        Control {
                            id: control
                            rotation: privacyFolderItem.isExpanded ? 180 : 0
                            Behavior on rotation {
                                NumberAnimation {
                                    duration: 200
                                }
                            }
                            contentItem: D.DciIcon {
                                name: "arrow_ordinary_down"
                                sourceSize: Qt.size(12, 12)
                                theme: D.DTK.themeType
                                palette: parent ? D.DTK.makeIconPalette(parent.palette) : D.DTK.makeIconPalette(control.palette)
                            }
                        }
                        
                    }
                }
            }
        }
    }
}
