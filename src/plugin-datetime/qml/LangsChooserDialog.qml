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

            ListView {
                id: itemsView
                property string checkedLang
                Layout.fillWidth: true
                height: 360
                clip: true
                snapMode: ListView.SnapOneItem
                Layout.leftMargin: 10
                Layout.rightMargin: 10
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
                    onCheckedChanged: {
                        if (checked)
                            itemsView.checkedLang = model.key
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


