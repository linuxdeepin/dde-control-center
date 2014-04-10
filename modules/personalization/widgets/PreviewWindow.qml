import QtQuick 2.1
import QtGraphicalEffects 1.0
import QtQuick.Window 2.1
import Deepin.Widgets 1.0
import QtQml.Models 2.1

Window {
    id: previewsWindow

    width: 300
    height: 500
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"

    property var themeObject
    property int pointer: 0
    property var previews: [
        "/usr/share/backgrounds/default_background.jpg"
    ]

    property bool showPreviewWindow: false

    function getPreviewPictures() {
        var pics = new Array()

        if (themeObject.name == "Custom"){
            var currentThemePreviewPath = themeObject.backgroundFile
        }
        else{
            var currentThemePreviewPath = themeObject.previewPath
        }
        if(currentThemePreviewPath){
            pics.push(currentThemePreviewPath)
        }

        var currenGtkTheme = themeObject.gtkTheme
        var currenGtkThemePreviewPath = dbusPreviewPath.GtkPath(currenGtkTheme)
        if(currenGtkThemePreviewPath){
            pics.push(currenGtkThemePreviewPath)
        }

        var currentIconTheme = themeObject.iconTheme
        var currentIconThemePreviewPath = dbusPreviewPath.IconPath(currentIconTheme)
        if(currentIconThemePreviewPath){
            pics.push(currentIconThemePreviewPath)
        }

        var currentCursorTheme = themeObject.cursorTheme
        var currentCursorThemePreviewPath = dbusPreviewPath.CursorPath(currentCursorTheme)
        if(currentCursorThemePreviewPath){
            pics.push(currentCursorThemePreviewPath)
        }

        return pics
    }

    function showWindow(themeObject){
        previewsWindow.themeObject = themeObject
        previewsWindow.previews = getPreviewPictures()
        previewsWindow.show()
        previewsWindow.showPreviewWindow = true
        rootWindow.clickedToHide = false
    }

    function hideWindow(){
        previewsWindow.hide()
        previewsWindow.showPreviewWindow = false
        hideToTrue.restart()
    }

    function showPrevious() {
        if (pointer > 0) {
            pointer--
        }
    }

    function showNext() {
        if (pointer < previews.length -1) {
            pointer++
        }
    }

    Timer{
        id: hideToTrue
        interval: 200
        running: false
        onTriggered: {
            rootWindow.clickedToHide = true
        }
    }

    Rectangle {
        color: dconstants.contentBgColor
        radius: 3
        border.width: 1
        border.color: dconstants.fgColor

        Item {
            id: title_bar
            width: parent.width
            height: 30
            
            DssH2 {
                text: "Theme preview: %1".arg(themeObject.name)
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
                    previewsWindow.hideWindow()
                }
            }
        }

        Rectangle {
            id: central_area

            width: parent.width - 2
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: title_bar.bottom
            anchors.bottom: bottom_bar.top

            ListView {
                id: view
                anchors.fill: parent
                model: previews.length
                preferredHighlightBegin: 0;
                preferredHighlightEnd: 0
                highlightRangeMode: ListView.StrictlyEnforceRange
                orientation: ListView.Horizontal
                snapMode: ListView.SnapOneItem; 
                flickDeceleration: 100
                cacheBuffer: 200
                currentIndex: pointer

                delegate: Item {
                    width: view.width; height: view.height
                    Image {
                        anchors.fill: parent
                        source: previews[index]
                    }
                }
            }

            Item{
                width: parent.width
                height: 20
                anchors.bottom: parent.bottom

                Row {
                    anchors.centerIn: parent
                    height: parent.height
                    spacing: 20

                    Repeater {
                        model: previews.length

                        Rectangle {
                            width: 10; height: 10
                            radius: 5
                            border.width: 1
                            border.color: Qt.rgba(0, 0, 0, 0.5)
                            color: view.currentIndex == index ? dconstants.activeColor : dconstants.fgColor

                            MouseArea {
                                width: 20; height: 20
                                anchors.centerIn: parent
                                onClicked: {
                                    view.currentIndex = index
                                    print(previews.length)
                                }
                            }
                        }
                    }
                }
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
                    previewsWindow.showPrevious()
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
                    previewsWindow.showNext()
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
            anchors.bottom: parent.bottom

            DssH2 {
                text: qsTr("Previews %1/%2").arg(previewsWindow.pointer + 1).arg(previewsWindow.previews.length)
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 10
            }

            DTextButton{
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 6
                text: dsTr("Apply")
            }

        }

        anchors.fill: parent
    }
}
