// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Window
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0

Loader {
    id: loader
    active: false
    required property var viewModel
    signal selected(string data)

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
                text: qsTr("Add new keyboard layout")
            }

            SearchEdit {
                id: searchEdit
                Layout.fillWidth: true
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                placeholder: qsTr("Search")
                onTextChanged: {
                    viewModel.setFilterWildcard(text)
                }
                onEditingFinished: {
                    viewModel.setFilterWildcard(text)
                }
            }

            ListView {
                id: itemsView
                property string checkedLang
                Layout.fillWidth: true
                height: 360
                clip: true
                model: loader.viewModel
                ButtonGroup {
                    id: langGroup
                }

                section.property: "section"
                section.criteria: ViewSection.FullString
                section.delegate: Rectangle {
                    width: ListView.view.width
                    height: childrenRect.height

                    required property string section

                    Text {
                        text: parent.section === " " ? qsTr("Current language keyboard layouts") : parent.section
                    }
                }

                delegate: CheckDelegate {
                    id: checkDelegate
                    implicitWidth: itemsView.width
                    text: model.display
                    hoverEnabled: true
                    ButtonGroup.group: langGroup
                    onCheckedChanged: {
                        if (checked) {
                            itemsView.checkedLang = model.display
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
                        selected(itemsView.checkedLang)
                        ddialog.close()
                    }
                }
            }
        }
        onClosing: {
            loader.active = false
            viewModel.setFilterWildcard("")
        }
    }
    onLoaded: {
        item.show()
    }
}
