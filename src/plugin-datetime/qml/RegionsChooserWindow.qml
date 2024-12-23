// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Window
import org.deepin.dtk 1.0

Loader {
    id: loader
    active: false
    required property var viewModel
    property int currentIndex: -1
    property string currentText
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
        DWindow.enabled: true
        DWindow.enableSystemResize: false
        DWindow.enableBlurWindow: true
        // ensure show in center of mainwindow
        flags: Qt.Dialog
        // default color is white
        color: active ? DTK.palette.window : DTK.inactivePalette.window

        Item {
            anchors.fill: parent
            SearchEdit {
                id: searchEdit
                anchors.top: parent.top
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter
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
                anchors.top: searchEdit.bottom
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter
                height: 500
                width: 180
                clip: true
                model: viewModel
                currentIndex: loader.currentIndex

                ButtonGroup {
                    id: regionGroup
                }

                delegate: CheckDelegate {
                    id: checkDelegate
                    implicitWidth: itemsView.width
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
        Qt.callLater(item.requestActivate);
    }
}
