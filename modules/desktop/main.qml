import QtQuick 2.1
import DBus.Com.Deepin.Daemon.Desktop 1.0
import "../widgets"

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
                desktop_icons_select_view.getDelegateInstanceAt(0).select()
            } else {
                desktop_icons_select_view.getDelegateInstanceAt(0).deselect()
            }
        }
        onShowHomeIconChanged: {
            if (showHomeIcon) {
                desktop_icons_select_view.getDelegateInstanceAt(1).select()
            } else {
                desktop_icons_select_view.getDelegateInstanceAt(1).deselect()
            }
        }
        onShowTrashIconChanged: {
            if (showTrashIcon) {
                desktop_icons_select_view.getDelegateInstanceAt(2).select()
            } else {
                desktop_icons_select_view.getDelegateInstanceAt(2).deselect()
            }
        }
        onShowDSCIconChanged: {
            if (showDSCIcon) {
                desktop_icons_select_view.getDelegateInstanceAt(3).select()
            } else {
                desktop_icons_select_view.getDelegateInstanceAt(3).deselect()
            }
        }

        onDockModeChanged: {
			if (dockMode == "default") {
				dock_display_select_view.getDelegateInstanceAt(0).select()
			}
			if (dockMode == "autohide") {
				dock_display_select_view.getDelegateInstanceAt(1).select()
			}
			if (dockMode == "keephidden") {
				dock_display_select_view.getDelegateInstanceAt(2).select()
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

        move: Transition {
            NumberAnimation { properties: "x,y"; duration: 100 }
        }

        RaisedPart {
            id: desktop_icons
            title: "桌面图标"

            Image {
                id: desktop_icons_arrow
                source: arrowPic

                property string arrowPic: desktop.arrowDown

                anchors {
                    right: parent.right
                    rightMargin: parent.rightPadding
                    verticalCenter: parent.verticalCenter
                }

                MouseArea {
                    anchors.fill: parent

                    onPressed: {
                        if (parent.arrowPic == desktop.arrowUp) {
                            parent.arrowPic = desktop.arrowDown
                            desktop_icons_select.visible = false
                        } else {
                            parent.arrowPic = desktop.arrowUp
                            desktop_icons_select.visible = true
                        }
                    }
                }
            }
        }
        ConcavePart {
            id:desktop_icons_select
            visible: false

            rows: 2
            MultipleSelectView {
                id: desktop_icons_select_view
                rows: 2
                columns: 2

                width: parent.width
                height: rows * 30

                model: ListModel {}
                Component.onCompleted: {
                    model.append({"label": "计算机", "selected": dde_desktop.showComputerIcon})
                    model.append({"label": "主文件夹", "selected": dde_desktop.showHomeIcon})
                    model.append({"label": "回收站", "selected": dde_desktop.showTrashIcon})
                    model.append({"label": "软件中心", "selected": dde_desktop.showDSCIcon})
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
        RaisedPart {
            title: "Dock显示"

            Image {
                id: dock_display_arrow
                source: arrowPic

                property string arrowPic: arrowDown

                anchors {
                    right: parent.right
                    rightMargin: parent.rightPadding
                    verticalCenter: parent.verticalCenter
                }

                MouseArea {
                    anchors.fill: parent

                    onPressed: {
                        if (parent.arrowPic == desktop.arrowUp) {
                            parent.arrowPic = desktop.arrowDown
                            dock_display_select.visible = false
                        } else {
                            parent.arrowPic = desktop.arrowUp
                            dock_display_select.visible = true
                        }
                    }
                }
            }
        }
        ConcavePart {
            id: dock_display_select
            visible: false

            rows: 1
            MultipleSelectView {
                id: dock_display_select_view
                width: parent.width
                height: rows * 30

                rows: 1
                columns: 3
                singleSelectionMode: true

                model: ListModel {}
                Component.onCompleted: {
                    model.append({"label": "正常模式", "selected": dde_desktop.dockMode == "default"})
                    model.append({"label": "自动隐藏", "selected": dde_desktop.dockMode == "autohide"})
                    model.append({"label": "一直隐藏", "selected": dde_desktop.dockMode == "keephidden"})
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
        RaisedPart {
            title: "屏幕热区"

            DRadioButton {
				buttonModel: ListModel {
					ListElement {
						buttonId: "top_left"
						buttonLabel: "左上角"
					}
					ListElement {
						buttonId: "bottom_right"
						buttonLabel: "右下角"
					}
				}

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
            visible: false

            rows: 1
            MultipleSelectView {
				id: hotspot_top_left_select_view
                width: parent.width
                height: rows * 30

                rows: 1
                columns: 3
				singleSelectionMode: true

                model: ListModel {}
                Component.onCompleted: {
                    model.append({"label": "无", "selected": dde_desktop.topLeft == 0})
                    model.append({"label": "打开的窗口", "selected": dde_desktop.topLeft == 1})
                    model.append({"label": "启动器", "selected": dde_desktop.topLeft == 2})
                }
                onSelect: {
					dde_desktop.SetTopLeftAction(index)
                }
            }
        }
        ConcavePart {
            id: hotspot_bottom_right_select
            visible: false

            rows: 1
            MultipleSelectView {
				id: hotspot_bottom_right_select_view
                width: parent.width
                height: rows * 30

                rows: 1
                columns: 3
				singleSelectionMode: true

                model: ListModel {}
               Component.onCompleted: {
                    model.append({"label": "无", "selected": dde_desktop.bottomRight == 0})
                    model.append({"label": "打开的窗口", "selected": dde_desktop.bottomRight == 1})
                    model.append({"label": "启动器", "selected": dde_desktop.bottomRight == 2})
                }
                onSelect: {
					dde_desktop.SetBottomRightAction(index)
                }
             }
        }
    }
}