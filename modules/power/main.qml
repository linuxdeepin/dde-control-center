import QtQuick 2.1
import Deepin.Daemon.Desktop 1.0

Rectangle {
    id: desktop

    color: root.defaultBackgroundColor
    property string arrowUp: "images/up.png"
    property string arrowDown: "images/down.png"
    property int topTitleHeight: 50

    property variant ddeDesktop: Desktop {}
	
    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width
        height: topTitleHeight
        color: root.defaultBackgroundColor
        
        Text {
            anchors.top: parent.top
            anchors.topMargin: 20
            anchors.left: parent.left
            anchors.leftMargin: 18
            color: "white"
            font.pixelSize: 15
            font.bold : true
            text: "电源管理"
        }
    }

    Column {
        anchors.top: parent.top
        anchors.topMargin: topTitleHeight

        move: Transition {
            NumberAnimation { properties: "x,y"; duration: 100 }
        }

        RaisedPart {
            id: desktop_icons
            title: "电源计划"

            function arrowClicked() {
                if (desktop_icons_arrow.arrowPic == desktop.arrowUp) {
                    desktop_icons_arrow.arrowPic = desktop.arrowDown
                    desktop_icons_select.visible = false
                } else {
                    desktop_icons_arrow.arrowPic = desktop.arrowUp
                    desktop_icons_select.visible = true
                }
            }

            Image {
                id: desktop_icons_arrow
                source: arrowPic
                property string arrowPic: desktop.arrowDown

                anchors {
                    right: parent.right
                    rightMargin: parent.rightPadding
                    verticalCenter: parent.verticalCenter
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: { desktop_icons.arrowClicked() }
            }

            Component.onCompleted: {
                arrowClicked()
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
					model.append({"label": "平衡", "selected": false})
					model.append({"label": "节能", "selected": false})
					model.append({"label": "高性能", "selected": false})
					model.append({"label": "自定义", "selected": true})
				}
				
				onSelect: {
					switch (index) {
						case 0:
						ddeDesktop.showComputerIcon = true
						break
						case 1:
						ddeDesktop.showHomeIcon = true
						break						
						case 2:
						ddeDesktop.showTrashIcon = true
						break						
						case 3:
						ddeDesktop.showDSCIcon = true
					}
				}
				onDeselect: {
					switch (index) {
						case 0:
						ddeDesktop.showComputerIcon = false
						break						
						case 1:
						ddeDesktop.showHomeIcon = false
						break						
						case 2:
						ddeDesktop.showTrashIcon = false
						break						
						case 3:
						ddeDesktop.showDSCIcon = false
					}
				}
            }
        }

        RaisedPart {
            title: "电源节能设置"

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
            title: "电源按钮配置"

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
