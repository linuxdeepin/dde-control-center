import QtQuick 2.1
import Deepin.Daemon.Desktop 1.0

Rectangle {
    id: desktop

    width: 310
    height: 600

    color: "#252627"

    property string arrowUp: "images/up.png"
    property string arrowDown: "images/down.png"
	
	Desktop {
		id: dde_desktop
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
                rows: 1
                columns: 3

                width: parent.width
                height: rows * 30

                model: ListModel {
                    ListElement {
                        label: "计算机"
                    }
                    ListElement {
                        label: "回收站"
                    }
                    ListElement {
                        label: "主文件夹"
                    }
                }
            }
        }
        RaisedPart {
            title: "屏幕热区"

            MultipleSelectButton {
                singleSelectionMode: true

                model: ListModel {
                    ListElement {
                        label: "左上角"
                    }
                    ListElement {
                        label: "右下角"
                    }
                }

                anchors {
                    right: parent.right
                }

                onSelect: {
                    /* console.log(index) */
                    if (index == 0) {
                        hotspot_top_left_select.visible = true
                    } else {
                        hotspot_bottom_right_select.visible = true
                    }
                }

                onDeselect: {
                    /* console.log(index) */
                    if (index == 0) {
                        hotspot_top_left_select.visible = false
                    } else {
                        hotspot_bottom_right_select.visible = false
                    }
                }
            }
        }
        ConcavePart {
            id: hotspot_top_left_select
            visible: false

            rows: 1
            MultipleSelectView {
                rows: 1
                columns: 3

                width: parent.width
                height: rows * 30

                model: ListModel {
                    ListElement {
                        label: "无"
                    }
                    ListElement {
                        label: "启动器"
                    }
                    ListElement {
                        label: "打开的窗口"
                    }
                }
            }
        }
        ConcavePart {
            id: hotspot_bottom_right_select
            visible: false

            rows: 1
           MultipleSelectView {
                rows: 1
                columns: 3

                width: parent.width
                height: rows * 30

                model: ListModel {
                    ListElement {
                        label: "无"
                    }
                    ListElement {
                        label: "启动器"
                    }
                    ListElement {
                        label: "打开的窗口"
                    }
                }
            }
        }
    }
}