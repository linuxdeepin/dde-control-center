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
            displayName: qsTr("Allow below apps to access these files and folders:")
        }

        DccRepeater {
            model: dccData.appsModel
            delegate: DccObject {
                id: privacyFolderItem
                name: "plugin" + model.name
                property real iconSize: 16
                property bool isExpaned: false
                property var dataModel: model
                parentName: "privacy/filefolder/filefolderViewGroup"
                weight: 10 + index * 10
                pageType: DccObject.Item
                visible: !model.noDisplay

                Connections {
                    target: parentItem
                    function onClicked() {
                        privacyFolderItem.isExpaned = !privacyFolderItem.isExpaned
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
                        }
                        DccLabel {
                            text: model.name
                        }
                        Item {
                            Layout.fillWidth: true
                        }
                        Control {
                            id: control
                            rotation: privacyFolderItem.isExpaned ? 180 : 0
                            Behavior on rotation {
                                NumberAnimation {
                                    duration: 200
                                }
                            }
                            contentItem: D.DciIcon {
                                name: "arrow_ordinary_down"
                                sourceSize: Qt.size(12, 12)
                                theme: D.DTK.themeType
                                palette: D.DTK.makeIconPalette(control.palette)
                            }
                        }
                        
                    }

                    Item {
                        Layout.fillWidth: true
                        implicitHeight: coLayout.implicitHeight
                        Behavior on implicitHeight {
                            NumberAnimation {
                                duration: 300
                                easing.type: Easing.OutQuart
                            }
                        }

                        ColumnLayout {
                            id: coLayout
                            anchors.fill: parent
                            Repeater {
                                id: rep
                                property var itemIndex: index

                                model: privacyFolderItem.isExpaned ? [
                                    { name: qsTr("Documents"), checked: privacyFolderItem.dataModel.documentPermission, premission: ApplicationItem.DocumentFoldersPermission},
                                    { name: qsTr("Desktop"), checked: privacyFolderItem.dataModel.desktopPermission, premission: ApplicationItem.DesktopFoldersPermission},
                                    { name: qsTr("Pictures"), checked: privacyFolderItem.dataModel.picturePermission, premission: ApplicationItem.PictureFoldersPermission},
                                    { name: qsTr("Videos"), checked: privacyFolderItem.dataModel.videoPermission, premission: ApplicationItem.VideoFoldersPermission},
                                    { name: qsTr("Music"), checked: privacyFolderItem.dataModel.musicPermission, premission: ApplicationItem.MusicFoldersPermission},
                                    { name: qsTr("Downloads"), checked: privacyFolderItem.dataModel.downloadPermission, premission: ApplicationItem.DownloadFoldersPermission}
                                    ] : []
                                

                                delegate: D.ItemDelegate {
                                    Layout.fillWidth: true
                                    cascadeSelected: true
                                    checkable: false
                                    contentFlow: true
                                    corners: getCornersForBackground(index, rep.model.count)
                                    content: RowLayout {
                                        spacing: 2
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
                                            checked: modelData.checked

                                            onCheckedChanged: {
                                                if (checked != modelData.checked) {
                                                    dccData.worker.setPremissionEnabled(rep.itemIndex, modelData.premission, checked)
                                                }
                                            }
                                        }
                                    }
                                    background: DccItemBackground {
                                        separatorVisible: true
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
