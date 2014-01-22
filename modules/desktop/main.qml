import QtQuick 2.1
import DBus.Com.Deepin.Daemon.Desktop 1.0
import Deepin.Widgets 1.0

Rectangle {
    id: desktop

    width: 310
    height: 600

    color: "#252627"

    property string arrowUp: "images/up.png"
    property string arrowDown: "images/down.png"

    Desktop {
        id: dde_desktop

        onShowComputerIconChanged: {
            if (showComputerIcon) {
                desktop_icons.selectView.getDelegateInstanceAt(0).select()
            } else {
                desktop_icons.selectView.getDelegateInstanceAt(0).deselect()
            }
        }
        onShowHomeIconChanged: {
            if (showHomeIcon) {
                desktop_icons.selectView.getDelegateInstanceAt(1).select()
            } else {
                desktop_icons.selectView.getDelegateInstanceAt(1).deselect()
            }
        }
        onShowTrashIconChanged: {
            if (showTrashIcon) {
                desktop_icons.selectView.getDelegateInstanceAt(2).select()
            } else {
                desktop_icons.selectView.getDelegateInstanceAt(2).deselect()
            }
        }
        onShowDSCIconChanged: {
            if (showDSCIcon) {
                desktop_icons.selectView.getDelegateInstanceAt(3).select()
            } else {
                desktop_icons.selectView.getDelegateInstanceAt(3).deselect()
            }
        }

        onDockModeChanged: {
            if (dockMode == "default") {
                dock_display.selectView.getDelegateInstanceAt(0).select()
            }
            if (dockMode == "autohide") {
                dock_display.selectView.getDelegateInstanceAt(1).select()
            }
            if (dockMode == "keephidden") {
                dock_display.selectView.getDelegateInstanceAt(2).select()
            }
        }

        onTopLeftChanged: {
            hotspot_top_left_select_view.getDelegateInstanceAt(topLeft).select()
        }
        onBottomRightChanged: {
            hotspot_bottom_right_select_view.getDelegateInstanceAt(bottomRight).select()
        }
    }

    Column {
        width: parent.width
        anchors.top: parent.top

        DssTitle { text: dsTr("Desktop") }

        DSeparatorHorizontal {}
        DBaseLine {}
        DBaseExpand {
            id: desktop_icons
            expanded: true
            property Item selectView
            
            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: dsTr("Dock Display")
                }
            }
            content.sourceComponent: DMultipleSelectView {
                id: desktop_icons_select_view
                rows: 2
                columns: 2

                width: parent.width
                height: rows * 30

                model: ListModel {}
                Component.onCompleted: {
                    desktop_icons.selectView = desktop_icons_select_view
                    model.append({"label": dsTr("Computer"), "selected": dde_desktop.showComputerIcon})
                    model.append({"label": dsTr("Home"), "selected": dde_desktop.showHomeIcon})
                    model.append({"label": dsTr("Trash"), "selected": dde_desktop.showTrashIcon})
                    model.append({"label": dsTr("DSC"), "selected": dde_desktop.showDSCIcon})
                }
                onSelect: {
                    switch (index) {
                        case 0:
                        dde_desktop.showComputerIcon = true
                        break
                        case 1:
                        dde_desktop.showHomeIcon = true
                        break
                        case 2:
                        dde_desktop.showTrashIcon = true
                        break
                        case 3:
                        dde_desktop.showDSCIcon = true
                    }
                }
                onDeselect: {
                    switch (index) {
                        case 0:
                        dde_desktop.showComputerIcon = false
                        break
                        case 1:
                        dde_desktop.showHomeIcon = false
                        break
                        case 2:
                        dde_desktop.showTrashIcon = false
                        break
                        case 3:
                        dde_desktop.showDSCIcon = false
                    }
                }
            }
        }
        DSeparatorHorizontal {}
        DBaseLine {}
        DBaseExpand {
            id: dock_display
            expanded: true
            property Item selectView
            
            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: dsTr("Dock Display")
                }
            }
            content.sourceComponent: DMultipleSelectView {
                id: dock_display_select_view
                width: parent.width
                height: rows * 30

                rows: 1
                columns: 3
                singleSelectionMode: true

                model: ListModel {}
                Component.onCompleted: {
                    dock_display.selectView = dock_display_select_view
                    model.append({"label": dsTr("Default Style"), "selected": dde_desktop.dockMode == "default"})
                    model.append({"label": dsTr("Auto Hide"), "selected": dde_desktop.dockMode == "autohide"})
                    model.append({"label": dsTr("Invisible"), "selected": dde_desktop.dockMode == "keephidden"})
                }
                onSelect: {
                    switch (index) {
                        case 0:
                        dde_desktop.dockMode = "default"
                        break
                        case 1:
                        dde_desktop.dockMode = "autohide"
                        break
                        case 2:
                        dde_desktop.dockMode = "keephidden"
                    }
                }
            }
        }
        DSeparatorHorizontal {}
        DBaseLine {}
        RaisedPart {
            title: dsTr("Hot Zone")

            DRadioButton {
                buttonModel: [
                    {"buttonId": "top_left", "buttonLabel": dsTr("Top Left")},
                    {"buttonId": "bottom_right", "buttonLabel": dsTr("Bottom Right")}
                ]

                anchors {
                    right: parent.right
                    rightMargin: parent.rightPadding
                    verticalCenter: parent.verticalCenter
                }

                onItemSelected: {
                    if (idx == 0) {
                        hotspot_top_left_select.visible = true
                        hotspot_bottom_right_select.visible = false
                    } else {
                        hotspot_top_left_select.visible = false
                        hotspot_bottom_right_select.visible = true
                    }
                }
            }
        }
        ConcavePart {
            id: hotspot_top_left_select
            visible: true

            rows: 1
            DMultipleSelectView {
                id: hotspot_top_left_select_view
                width: parent.width
                height: rows * 30

                rows: 1
                columns: 3
                singleSelectionMode: true

                model: ListModel {}
                Component.onCompleted: {
                    model.append({"label": dsTr("Nothing"), "selected": dde_desktop.topLeft == 0})
                    model.append({"label": dsTr("Opening Window"), "selected": dde_desktop.topLeft == 1})
                    model.append({"label": dsTr("Launcher"), "selected": dde_desktop.topLeft == 2})
                }
                onSelect: {
                    dde_desktop.topLeft = index
                }
            }
        }
        ConcavePart {
            id: hotspot_bottom_right_select
            visible: false

            rows: 1
            DMultipleSelectView {
                id: hotspot_bottom_right_select_view
                width: parent.width
                height: rows * 30

                rows: 1
                columns: 3
                singleSelectionMode: true

                model: ListModel {}
                Component.onCompleted: {
                    model.append({"label": dsTr("Nothing"), "selected": dde_desktop.bottomRight == 0})
                    model.append({"label": dsTr("Opening Window"), "selected": dde_desktop.bottomRight == 1})
                    model.append({"label": dsTr("Launcher"), "selected": dde_desktop.bottomRight == 2})
                }
                onSelect: {
                    dde_desktop.bottomRight = index
                }
            }
        }
    }
}
