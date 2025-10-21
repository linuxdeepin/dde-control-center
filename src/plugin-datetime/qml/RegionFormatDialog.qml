// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls 2.0
import QtQuick.Window
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc 1.0
import QtQml.Models
import QtQuick.Effects

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

    sourceComponent: D.DialogWindow {
        id: ddialog
        width: 738
        minimumWidth: width
        minimumHeight: 636
        maximumWidth: minimumWidth
        icon: "preferences-system"
        modality: Qt.WindowModal
        ColumnLayout {
            spacing: 10
            width: parent.width
            Label {
                Layout.alignment: Qt.AlignHCenter
                font.family: D.DTK.fontManager.t5.family
                font.bold: true
                font.pixelSize: D.DTK.fontManager.t5.pixelSize
                text: qsTr("Regional format")
            }

            RowLayout {
                spacing: 10
                ColumnLayout {
                    Layout.preferredWidth: 348
                    Layout.maximumWidth: 348
                    spacing: 10
                    D.SearchEdit {
                        id: searchEdit
                        Layout.fillWidth: true
                        Layout.leftMargin: 0
                        Layout.rightMargin: 0
                        placeholder: qsTr("Search")
                        font: D.DTK.fontManager.t6
                        onTextChanged: {
                            viewModel.setFilterWildcard(text);
                            itemsView.positionViewAtBeginning();
                        }
                        onEditingFinished: {
                            viewModel.setFilterWildcard(text);
                            itemsView.positionViewAtBeginning();
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Rectangle {
                            id: backgroundRect
                            anchors.fill: parent
                            color: palette.base
                            radius: 8
                            border.width: 0
                        }

                        Item {
                            id: listMask
                            anchors.fill: parent
                            layer.enabled: true
                            visible: false
                            Rectangle {
                                anchors.fill: parent
                                anchors.margins: 0.5
                                radius: 8
                            }
                        }

                        D.ListView {
                            id: itemsView
                            property string checkedLang
                            property string checkedLocale
                            property string selectedLangKey: ""
                            property string selectedLocaleKey: ""
                            anchors.fill: parent
                            model: regionFormatLoader.viewModel
                            currentIndex: regionFormatLoader.currentIndex
                            clip: true

                            layer.enabled: true
                            layer.effect: MultiEffect {
                                maskEnabled: true
                                maskSource: listMask
                                antialiasing: true
                                maskThresholdMin: 0.5
                                maskSpreadAtMin: 1.0
                            }

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

                            delegate: D.CheckDelegate {
                                id: checkDelegate
                                implicitWidth: itemsView.width
                                text: model.display
                                font: D.DTK.fontManager.t6
                                checked: (itemsView.selectedLangKey === model.langKey &&
                                         itemsView.selectedLocaleKey === model.localeKey) ||
                                        (itemsView.selectedLangKey === "" && index === itemsView.currentIndex)
                                hoverEnabled: true
                                ButtonGroup.group: langGroup

                                background: Control {
                                    implicitWidth: DS.Style.itemDelegate.width
                                    implicitHeight: DS.Style.itemDelegate.height

                                    // Hover background
                                    Rectangle {
                                        anchors.fill: parent
                                        visible: !checkDelegate.checked && !D.DTK.hasAnimation && checkDelegate.hovered
                                        color: checkDelegate.D.ColorSelector.backgroundColor
                                    }

                                    // Selected background
                                    Rectangle {
                                        anchors.fill: parent
                                        visible: checkDelegate.checked
                                        color: DS.Style.itemDelegate.checkedColor
                                    }
                                }

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
                        Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
                        Layout.preferredHeight: searchEdit.height
                        Layout.bottomMargin: 10
                        text: qsTr("Default formats")
                        font: D.DTK.fontManager.t6
                        verticalAlignment: Text.AlignBottom
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
                            font: D.DTK.fontManager.t6
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
                                radius: 8 + bgMargins
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
                Layout.bottomMargin: 6
                spacing: 10
                D.Button {
                    font: D.DTK.fontManager.t6
                    text: qsTr("Cancel")
                    onClicked: {
                        ddialog.close()
                    }
                }
                D.Button {
                    text: qsTr("Save")
                    font: D.DTK.fontManager.t6
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
