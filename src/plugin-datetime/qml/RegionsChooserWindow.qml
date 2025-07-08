// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Window
import org.deepin.dtk 1.0
import QtQuick.Layouts 1.0

Loader {
    id: loader
    active: false
    required property var viewModel
    property int currentIndex: -1
    property string currentText
    property point mousePosition: Qt.point(0, 0)
    signal selectedRegion(string region)

    function show() {
        active = true
    }

    function isVisible() {
        return active
    }

    sourceComponent: Window {
        id: searchWindow
        width: 200
        height: 500
        minimumWidth: width
        minimumHeight: height
        maximumWidth: minimumWidth
        maximumHeight: minimumHeight
        DWindow.enabled: true
        DWindow.enableSystemResize: false
        DWindow.enableBlurWindow: true
        // ensure show in center of mainwindow
        flags: Qt.Dialog
        // default color is white
        color: active ? DTK.palette.window : DTK.inactivePalette.window

        ColumnLayout {
            id: contentLayout
            anchors.fill: parent
            anchors.margins: 6
            SearchEdit {
                id: searchEdit
                implicitHeight: 30
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop
                placeholder: qsTr("Search")
                palette: DTK.palette
                onTextChanged: {
                    viewModel.setFilterWildcard(text);
                }
                onEditingFinished: {
                    viewModel.setFilterWildcard(text);
                }
            }

            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                id: itemsView
                property string checkedLang
                property string checkedLocale
                clip: true
                model: viewModel
                currentIndex: loader.currentIndex

                ScrollBar.vertical: ScrollBar {
                    parent: itemsView
                    anchors.top: itemsView.top
                    anchors.right: itemsView.right
                    anchors.bottom: itemsView.bottom
                    width: 10
                    implicitWidth: 10
                }

                ButtonGroup {
                    id: regionGroup
                }

                delegate: CheckDelegate {
                    id: checkDelegate
                    implicitWidth: itemsView.width - itemsView.leftMargin - itemsView.rightMargin
                    palette: DTK.palette
                    text: model.display
                    checked: text === loader.currentText
                    hoverEnabled: true
                    ButtonGroup.group: regionGroup
                    onCheckedChanged: {
                        if (checked && loader.currentText !== model.display) {
                            selectedRegion(model.display)
                            closeWindow()
                        }
                    }
                }
                Component.onCompleted: {
                    if (currentIndex >= 0) {
                        let delegateHeight =  40
                        contentY = currentIndex * delegateHeight;
                        console.log("currentIndex", currentIndex, contentY)
                    }
                }
            }
        }

        function closeWindow() {
            loader.viewModel.setFilterWildcard("");
            searchWindow.close()
            loader.active = false
        }

        onActiveFocusItemChanged: {
            if (!activeFocusItem) {
                searchWindow.closeWindow()
            }
        }
        onActiveChanged: {
            if (!active) {
                searchWindow.closeWindow()
            }
        }
    }

    onLoaded: {
        item.show()
        if (loader.mousePosition.x !== 0 || loader.mousePosition.y !== 0) {
            item.x = loader.mousePosition.x
            item.y = loader.mousePosition.y
        }
        Qt.callLater(item.requestActivate);
    }
}
