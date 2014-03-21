import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Widgets 1.0

Window {
    id: root

    width: 300
    height: 500
    flags: Qt.FramelessWindowHint
    color: "transparent"

    property int pointer: 0
    property var previews: [
        "/usr/share/backgrounds/default_background.jpg"
    ]

    property bool showPreviewWindow: false

    function showWindow(){
        root.show()
        root.showPreviewWindow = true
        rootWindow.clickedToHide = false
    }

    function hideWindow(){
        root.hide()
        root.showPreviewWindow = false
        rootWindow.clickedToHide = true
    }

    function showPrevious() {
        if (pointer > 0) {
            pointer--
        }
        else{
            pointer = previews.length - 1
        }
        preview_image.source = previews[pointer]
    }

    function showNext() {
        if (pointer < previews.length -1) {
            pointer++
        }
        else{
            pointer = 0
        }
        preview_image.source = previews[pointer]
    }

    DWindowFrame {

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

            DImageButton {
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.topMargin: 8
                anchors.rightMargin: 8

                normal_image: "images/close_normal.png"
                hover_image: "images/close_normal.png"
                press_image: "images/close_press.png"

                onClicked: {
                    root.hideWindow()
                }
            }
        }

        Rectangle {
            id: central_area
            color: "grey"

            width: parent.width - 2
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: title_bar.bottom
            anchors.bottom: bottom_bar.top

            Image {
                id: preview_image
                anchors.fill: parent
                source: previews[root.pointer]
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
