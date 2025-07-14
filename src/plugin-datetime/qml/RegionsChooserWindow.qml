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

            ArrowListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                id: itemsView
                property string checkedRegion
                clip: true
                maxVisibleItems: 12
                view.model: viewModel
                view.currentIndex: loader.currentIndex

                Component {
                    id: scrollBarComponent
                    ScrollBar {
                        parent: itemsView.view
                        anchors.top: itemsView.view.top
                        anchors.right: itemsView.view.right
                        anchors.bottom: itemsView.view.bottom
                        width: 10
                        implicitWidth: 10
                    }
                }

                ButtonGroup {
                    id: regionGroup
                }

                view.delegate: MenuItem {
                    id: menuItem
                    implicitWidth: itemsView.width
                    implicitHeight: 30
                    text: model.display
                    checkable: true
                    checked: text === loader.currentText
                    hoverEnabled: true
                    highlighted: hovered
                    autoExclusive: true
                    ButtonGroup.group: regionGroup
                    useIndicatorPadding: true

                    onCheckedChanged: {
                        if (checked && loader.currentText !== model.display) {
                            selectedRegion(model.display)
                            closeWindow()
                        }
                    }
                }

                Component.onCompleted: {
                    // Add ScrollBar to the internal view
                    if (view) {
                        view.ScrollBar.vertical = scrollBarComponent.createObject(view)
                    }
                    // Set initial scroll position
                    if (currentIndex >= 0) {
                        let delegateHeight = 30
                        view.contentY = currentIndex * delegateHeight;
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
