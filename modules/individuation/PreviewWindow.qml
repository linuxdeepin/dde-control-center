import QtQuick 2.1
import QtQuick.Window 2.1
import "../widgets"

Window {
    x: 1000
    y: 100
    width: 300
    height: 500
    flags: Qt.Popup
    color: "transparent"

    property variant previews: [
        "/home/hualet/Pictures/wallpapers-collect/firefox.jpg",
        "/home/hualet/Pictures/wallpapers-collect/xingkong.jpg",
        "/home/hualet/Pictures/wallpapers-collect/yuzhou17.jpg",
        "/home/hualet/Pictures/wallpapers-collect/test.jpg",
        "/home/hualet/Pictures/wallpapers-collect/Unix.jpg"
    ]

    Rectangle {
        radius: 8
        color: "#1A1B1B"

        Item {
            id: title_bar
            width: parent.width
            height: 30

            DssH2 {
                text: "Theme preview"
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.topMargin: 8
                anchors.leftMargin: 10
            }
        }

        Rectangle {
            id: central_area
            color: "grey"

            Image {
                id: preview_image
                source: previews[0]

                anchors.fill: parent
                anchors.topMargin: 10
                anchors.bottomMargin: 10
                anchors.leftMargin: 10
                anchors.rightMargin: 10
            }

            Image {
                id: cursor_image
                visible: false
                source: "images/close_normal.png"
            }

            MouseArea {
                id: left_area
                width: parent.width / 2
                height: parent.height
                hoverEnabled: true

                onEntered: {
                    cursorShape = Qt.BlankCursor
                    cursor_image.visible = true
                }

                onPositionChanged: {
                    var pos = mapToItem(parent, mouse.x, mouse.y)
                    cursor_image.x = pos.x
                    cursor_image.y = pos.y
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
                    cursor_image.visible = true
                }

                onPositionChanged: {
                    var pos = mapToItem(parent, mouse.x, mouse.y)
                    cursor_image.x = pos.x
                    cursor_image.y = pos.y
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

            anchors.bottom: parent.bottom
        }

        anchors.fill: parent
    }
}