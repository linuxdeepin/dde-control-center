// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Window
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0

Loader {
    id: langDialogLoader
    active: false
    required property var viewModel
    signal selectedLang(string lang)

    sourceComponent: DialogWindow {
        id: ddialog
        width: 420
        minimumWidth: width
        minimumHeight: height
        maximumWidth: minimumWidth
        maximumHeight: minimumHeight
        icon: "preferences-system"
        modality: Qt.WindowModal
        ColumnLayout {
            spacing: 10
            width: parent.width
            Label {
                Layout.alignment: Qt.AlignHCenter
                font.bold: true
                text: qsTr("Add language")
            }

            SearchEdit {
                id: searchEdit
                Layout.fillWidth: true
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                placeholder: qsTr("Search")
                onVisibleChanged: {
                    clear()
                }
                onTextChanged: {
                    viewModel.setFilterWildcard(text);
                }
                onEditingFinished: {
                    viewModel.setFilterWildcard(text);
                }
            }

            RowLayout {
                Layout.fillWidth: true
                height: 360
                Layout.leftMargin: 10
                Layout.rightMargin: 0
                spacing: 0

                Item {
                    Layout.fillWidth: true
                    height: parent.height

                    ListView {
                        id: itemsView
                        property string checkedLang
                        anchors.fill: parent
                        clip: true
                        snapMode: ListView.SnapOneItem
                        model: langDialogLoader.viewModel

                        ButtonGroup {
                            id: langGroup
                        }

                        delegate: CheckDelegate {
                            id: checkDelegate
                            implicitWidth: itemsView.width
                            text: model.display
                            hoverEnabled: true
                            ButtonGroup.group: langGroup

                            indicator: Loader {
                                x: checkDelegate.mirrored ? checkDelegate.leftPadding : checkDelegate.width - width - checkDelegate.rightPadding
                                y: checkDelegate.topPadding + (checkDelegate.availableHeight - height) / 2
                                active: indicatorVisible

                                sourceComponent: DciIcon {
                                    palette: checkDelegate.DTK.makeIconPalette(checkDelegate.palette)
                                    mode: checkDelegate.ColorSelector.controlState
                                    theme: checkDelegate.ColorSelector.controlTheme
                                    name: indicatorIcon
                                    sourceSize: Qt.size(16, 16)
                                    fallbackToQIcon: false
                                    onNameChanged: {
                                        play(DTK.NormalState);
                                    }
                                    Component.onCompleted: {
                                        if (indicatorVisible)
                                            play(DTK.NormalState);
                                    }
                                }
                            }

                            contentItem: RowLayout {
                                Loader {
                                    id: labelLoader
                                    property string text: checkDelegate.text
                                    property bool shouldSplit: text.split("-").length === 2
                                    Layout.fillWidth: !checkDelegate.content
                                    sourceComponent: shouldSplit ? splitComponent : singleComponent

                                    Component {
                                        id: singleComponent
                                        IconLabel {
                                            spacing: checkDelegate.spacing
                                            mirrored: checkDelegate.mirrored
                                            display: checkDelegate.display
                                            alignment: Qt.AlignLeft | Qt.AlignVCenter
                                            text: labelLoader.text
                                            font: checkDelegate.font
                                            color: checkDelegate.palette.windowText
                                            icon: DTK.makeIcon(checkDelegate.icon, checkDelegate.DciIcon)
                                            Layout.fillWidth: !checkDelegate.content
                                        }
                                    }

                                    Component {
                                        id: splitComponent
                                        RowLayout {
                                            spacing: 0
                                            IconLabel {
                                                mirrored: checkDelegate.mirrored
                                                display: checkDelegate.display
                                                alignment: Qt.AlignLeft | Qt.AlignVCenter
                                                text: labelLoader.text.split("-")[0] || ""
                                                font: checkDelegate.font
                                                color: checkDelegate.palette.windowText
                                            }
                                            IconLabel {
                                                mirrored: checkDelegate.mirrored
                                                display: checkDelegate.display
                                                alignment: Qt.AlignLeft | Qt.AlignVCenter
                                                text: "-"
                                                font: checkDelegate.font
                                                color: checkDelegate.palette.windowText
                                            }
                                            IconLabel {
                                                mirrored: checkDelegate.mirrored
                                                display: checkDelegate.display
                                                alignment: Qt.AlignLeft | Qt.AlignVCenter
                                                text: labelLoader.text.split("-")[1] || ""
                                                font: checkDelegate.font
                                                color: checkDelegate.palette.windowText
                                                icon: DTK.makeIcon(checkDelegate.icon, checkDelegate.DciIcon)
                                                Layout.fillWidth: !checkDelegate.content
                                            }
                                        }
                                    }
                                }
                                Loader {
                                    active: checkDelegate.content
                                    sourceComponent: checkDelegate.content
                                    Layout.fillWidth: true
                                }
                            }
                            onCheckedChanged: {
                                if (checked)
                                    itemsView.checkedLang = model.key
                            }
                        }
                    }
                }

                Item {
                    width: 10
                    Layout.preferredWidth: 10
                    Layout.fillHeight: true

                    ScrollBar {
                        anchors.fill: parent
                        orientation: Qt.Vertical

                        position: itemsView.visibleArea.yPosition
                        size: itemsView.visibleArea.heightRatio
                        active: itemsView.moving || itemsView.flicking

                        onPositionChanged: {
                            if (pressed) {
                                itemsView.contentY = position * (itemsView.contentHeight - itemsView.height)
                            }
                        }
                    }
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 10
                Button {
                    Layout.bottomMargin: 10
                    text: qsTr("Cancel")
                    onClicked: {
                        ddialog.close()
                    }
                }
                Button {
                    Layout.bottomMargin: 10
                    text: qsTr("Add")
                    enabled: itemsView.checkedLang.length > 0
                    onClicked: {
                        selectedLang(itemsView.checkedLang)
                        ddialog.close()
                    }
                }
            }
        }
        onClosing: {
            langDialogLoader.active = false
        }
    }
    onLoaded: {
        item.show()
    }
    onActiveChanged: {
        if (!active) {
            viewModel.setFilterWildcard("");
        }
    }
}


