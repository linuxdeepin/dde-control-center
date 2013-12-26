import QtQuick 2.1
import QtQuick.Window 2.1
import "../widgets"

Window {
    id: root

    width: 300
    height: 500
    flags: Qt.FramelessWindowHint
    color: "transparent"

    property int pointer: 0
    property variant previews: [
        "/home/hualet/Pictures/wallpapers-collect/firefox.jpg",
        "/home/hualet/Pictures/wallpapers-collect/xingkong.jpg",
        "/home/hualet/Pictures/wallpapers-collect/yuzhou17.jpg",
        "/home/hualet/Pictures/wallpapers-collect/test.jpg",
        "/home/hualet/Pictures/wallpapers-collect/Unix.png"
    ]

    function showPrevious() {
        if (pointer > 0) {
            pointer--
        }
        preview_image.source = previews[pointer]
    }

    function showNext() {
        if (pointer < previews.length -1) {
            pointer++
        }
        preview_image.source = previews[pointer]
    }

    Rectangle {
        radius: 8
        color: "#1A1B1B"

        Item {
            id: title_bar
            width: parent.width
            height: 30
            
            MouseArea {
                anchors.fill: parent

                property int startX
                property int startY
                property bool holdFlag
                onPressed: {
                    startX = mouse.x;
                    startY = mouse.y;
                    holdFlag = true;
                }
                onReleased: holdFlag = false;
                onPositionChanged: {
                    if (holdFlag) {
                        root.setX(root.x + mouse.x - startX)
                        root.setY(root.y + mouse.y - startY)
                    }
                }
            }            

            DssH2 {
                text: "Theme preview"
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.topMargin: 8
                anchors.leftMargin: 10
            }

            DImageButton {
                normal_image: "images/close_normal.png"
                hover_image: "images/close_normal.png"
                press_image: "images/close_press.png"

                onClicked: {
                    root.hide()
                }
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.topMargin: 8
                anchors.rightMargin: 8
            }
        }

        Rectangle {
            id: central_area
            color: "grey"

            Image {
                id: preview_image
                source: previews[root.pointer]

                anchors.fill: parent
                anchors.topMargin: 10
                anchors.bottomMargin: 10
                anchors.leftMargin: 10
                anchors.rightMargin: 10
            }

            Image {
                id: cursor_image
                visible: false
            }

            MouseArea {
                id: left_area
                width: parent.width / 2
                height: parent.height
                hoverEnabled: true

                onEntered: {
                    cursorShape = Qt.BlankCursor
                    cursor_image.source = "images/left_normal.png"
                    cursor_image.visible = true
                }

                onPositionChanged: {
                    var pos = mapToItem(parent, mouse.x, mouse.y)
                    cursor_image.x = pos.x
                    cursor_image.y = pos.y
                }

                onPressed: {
                    cursor_image.source = "images/left_pressed.png"
                }

                onReleased: {
                    cursor_image.source = "images/left_normal.png"
                    root.showPrevious()
                }

                onExited: {
                    cursorShape = Qt.ArrowCursor
                    cursor_image.visible = false
                }
            }

            MouseArea {
                id: right_area
                width: parent.width / 2
                height: parent.height
                hoverEnabled: true
                anchors.right: parent.right

                onEntered: {
                    cursorShape = Qt.BlankCursor
                    cursor_image.source = "images/right_normal.png"
                    cursor_image.visible = true
                }

                onPositionChanged: {
                    var pos = mapToItem(parent, mouse.x, mouse.y)
                    cursor_image.x = pos.x
                    cursor_image.y = pos.y
                }

                onPressed: {
                    cursor_image.source = "images/right_pressed.png"
                }

                onReleased: {
                    cursor_image.source = "images/right_normal.png"
                    root.showNext()
                }

                onExited: {
                    cursorShape = Qt.ArrowCursor
                    cursor_image.visible = false
                }
            }

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: title_bar.bottom
            anchors.bottom: bottom_bar.top
        }

        Item {
            id: bottom_bar
            width: parent.width
            height: 40

            DssH2 {
                text: qsTr("Previews %1/%2").arg(root.pointer + 1).arg(root.previews.length)
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.topMargin: 8
                anchors.leftMargin: 10
            }

            anchors.bottom: parent.bottom
        }

        anchors.fill: parent
    }
}