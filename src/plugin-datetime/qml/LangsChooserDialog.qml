// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Window
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0
import org.deepin.dtk.style 1.0 as DS

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
                font.family: DTK.fontManager.t5.family
                font.bold: true
                font.pixelSize: DTK.fontManager.t5.pixelSize
                text: qsTr("Add language")
            }

            SearchEdit {
                id: searchEdit
                implicitHeight: Math.max(30, searchEditFontMetrics.height + (DS.Style.control.padding - DS.Style.control.borderWidth) * 2) // Minimum 30px, adaptive based on font
                Layout.fillWidth: true
                Layout.leftMargin: 6 - DS.Style.dialogWindow.contentHMargin
                Layout.rightMargin: 6 - DS.Style.dialogWindow.contentHMargin
                placeholder: qsTr("Search")
                font: DTK.fontManager.t6
                onVisibleChanged: {
                    clear()
                }
                onTextChanged: {
                    viewModel.setFilterWildcard(text);
                }
                onEditingFinished: {
                    viewModel.setFilterWildcard(text);
                }

                FontMetrics {
                    id: searchEditFontMetrics
                    font: searchEdit.font
                }
            }

            RowLayout {
                Layout.fillWidth: true
                height: 360
                Layout.leftMargin: 6 - DS.Style.dialogWindow.contentHMargin
                Layout.rightMargin: -DS.Style.dialogWindow.contentHMargin
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
                            implicitHeight: Math.max(30, checkDelegateFontMetrics.height + (DS.Style.control.padding - DS.Style.control.borderWidth) * 2) // Minimum 30px, adaptive based on font
                            text: model.display
                            hoverEnabled: true
                            ButtonGroup.group: langGroup
                            topPadding: 0
                            bottomPadding: 0
                            font: DTK.fontManager.t6

                            FontMetrics {
                                id: checkDelegateFontMetrics
                                font: checkDelegate.font
                            }

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
                                    Layout.leftMargin: 6
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
                                            Layout.alignment: Qt.AlignVCenter
                                        }
                                    }

                                    Component {
                                        id: splitComponent
                                        RowLayout {
                                            spacing: 0
                                            Layout.alignment: Qt.AlignVCenter
                                            IconLabel {
                                                mirrored: checkDelegate.mirrored
                                                display: checkDelegate.display
                                                alignment: Qt.AlignLeft | Qt.AlignVCenter
                                                text: labelLoader.text.split("-")[0] || ""
                                                font: checkDelegate.font
                                                color: checkDelegate.palette.windowText
                                                Layout.alignment: Qt.AlignVCenter
                                            }
                                            IconLabel {
                                                mirrored: checkDelegate.mirrored
                                                display: checkDelegate.display
                                                alignment: Qt.AlignLeft | Qt.AlignVCenter
                                                text: "-"
                                                font: checkDelegate.font
                                                color: checkDelegate.palette.windowText
                                                Layout.alignment: Qt.AlignVCenter
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
                                                Layout.alignment: Qt.AlignVCenter
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
                    width: 7
                    Layout.preferredWidth: 7
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
                Layout.fillWidth: true
                Layout.leftMargin: 6 - DS.Style.dialogWindow.contentHMargin
                Layout.rightMargin: 6 - DS.Style.dialogWindow.contentHMargin
                spacing: 6
                Button {
                    id: cancelButton
                    Layout.fillWidth: true
                    Layout.bottomMargin: 6
                    implicitHeight: Math.max(30, cancelButtonFontMetrics.height + (DS.Style.control.padding - DS.Style.control.borderWidth) * 2) // Minimum 30px, adaptive based on font
                    text: qsTr("Cancel")
                    onClicked: {
                        ddialog.close()
                    }

                    FontMetrics {
                        id: cancelButtonFontMetrics
                        font: cancelButton.font
                    }
                }
                Button {
                    id: addButton
                    Layout.fillWidth: true
                    Layout.bottomMargin: 6
                    implicitHeight: Math.max(30, addButtonFontMetrics.height + (DS.Style.control.padding - DS.Style.control.borderWidth) * 2) // Minimum 30px, adaptive based on font
                    text: qsTr("Add")
                    enabled: itemsView.checkedLang.length > 0
                    onClicked: {
                        selectedLang(itemsView.checkedLang)
                        ddialog.close()
                    }

                    FontMetrics {
                        id: addButtonFontMetrics
                        font: addButton.font
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


