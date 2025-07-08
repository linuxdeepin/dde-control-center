// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls 2.0
import QtQuick.Window
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0
import org.deepin.dcc 1.0
import QtQml.Models

Loader {
    id: regionFormatLoader
    active: false
    required property var viewModel
    property int currentIndex: -1

    signal selectedRegion(string locale, string lang)

    function show() {
        viewModel.setFilterWildcard("");
        active = true
    }

    sourceComponent: DialogWindow {
        id: ddialog
        width: 738
        height: 636
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
                text: qsTr("Regions and formats")
            }

            RowLayout {
                ColumnLayout {
                    Layout.preferredWidth: 348
                    Layout.maximumWidth: 348
                    SearchEdit {
                        id: searchEdit
                        Layout.fillWidth: true
                        Layout.leftMargin: 10
                        Layout.rightMargin: 10
                        placeholder: qsTr("Search")
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
                        property string checkedLocale
                        property string selectedLangKey: ""
                        property string selectedLocaleKey: ""
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        model: regionFormatLoader.viewModel
                        currentIndex: regionFormatLoader.currentIndex

                        Component.onCompleted: {
                            if (currentIndex >= 0 && currentIndex < count) {
                                let delegateHeight = 40;
                                contentY = currentIndex * delegateHeight;

                                Qt.callLater(function() {
                                    if (currentItem && currentItem.model) {
                                        selectedLangKey = currentItem.model.langKey;
                                        selectedLocaleKey = currentItem.model.localeKey;
                                    }
                                });
                            }
                        }

                        ButtonGroup {
                            id: langGroup
                        }

                        delegate: CheckDelegate {
                            id: checkDelegate
                            implicitWidth: itemsView.width
                            text: model.display
                            checked: (itemsView.selectedLangKey === model.langKey &&
                                     itemsView.selectedLocaleKey === model.localeKey) ||
                                    (itemsView.selectedLangKey === "" && index === itemsView.currentIndex)
                            hoverEnabled: true
                            ButtonGroup.group: langGroup
                            onCheckedChanged: {
                                if (checked) {
                                    itemsView.selectedLangKey = model.langKey;
                                    itemsView.selectedLocaleKey = model.localeKey;

                                    itemsView.checkedLang = model.langKey
                                    itemsView.checkedLocale = model.localeKey
                                    let idx = 0
                                    repeater.values[idx++].value = model.firstDay
                                    repeater.values[idx++].value = model.shortDate
                                    repeater.values[idx++].value = model.longDate
                                    repeater.values[idx++].value = model.shortTime
                                    repeater.values[idx++].value = model.longTime
                                    repeater.values[idx++].value = model.currency
                                    repeater.values[idx++].value = model.digit
                                    repeater.values[idx++].value = model.paperSize

                                    // TODO: any better way to reset model?
                                    repeater.model = 0
                                    repeater.model = repeater.values
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    implicitWidth: 1
                    implicitHeight: 500
                    color: palette.mid
                }

                ColumnLayout {
                    Layout.fillHeight: true
                    Layout.preferredWidth: 348
                    Layout.maximumWidth: 348
                    spacing: 0
                    Label {
                        Layout.alignment: Qt.AlignLeft | Qt.AlignTop | Qt.H
                        Layout.bottomMargin: 20
                        text: qsTr("Default formats")
                    }

                    Repeater {
                        id: repeater
                        property var values:[
                            { name: qsTr("First day of week"), value: "" },
                            { name: qsTr("Short date"), value: "" },
                            { name: qsTr("Long date"), value: "" },
                            { name: qsTr("Short time"), value: "" },
                            { name: qsTr("Long time"), value: "" },
                            { name: qsTr("Currency symbol"), value: "" },
                            { name: qsTr("Digit"), value: "" },
                            { name: qsTr("Paper size"), value: "" },
                        ]
                        model: values
                        function getName(index) {
                            return values.length > index ? values[index].name : ""
                        }
                        function getValue(index) {
                            return values.length > index ? values[index].value : ""
                        }

                        ItemDelegate {
                            id: root
                            Layout.fillWidth: true
                            backgroundVisible: true
                            checkable: false
                            topPadding: topInset
                            bottomPadding: bottomInset
                            contentFlow: true
                            corners: getCornersForBackground(index, repeater.count)
                            content: RowLayout {
                                ColumnLayout {
                                    Layout.leftMargin: 8
                                    Layout.fillWidth: true
                                    Layout.alignment: Qt.AlignVCenter
                                    spacing: 0
                                    DccLabel {
                                        Layout.fillWidth: true
                                        text: repeater.getName(index)
                                    }
                                }
                                Control {
                                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                    Layout.rightMargin: 10
                                    contentItem:  Label {
                                        text: repeater.getValue(index)
                                    }
                                }
                            }
                            background: DccItemBackground {
                                id: background
                                backgroundType: DccObject.Normal
                                separatorVisible: true
                                radius: 8
                            }
                        }
                    }
                    Item {
                        Layout.fillHeight: true
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
                    text: qsTr("Save")
                    enabled: itemsView.checkedLang.length > 0
                    onClicked: {
                        regionFormatLoader.selectedRegion(itemsView.checkedLocale, itemsView.checkedLang)
                        ddialog.close()
                    }
                }
            }
        }
        onClosing: {
            regionFormatLoader.active = false
        }
    }
    onLoaded: {
        item.show()
    }
}
