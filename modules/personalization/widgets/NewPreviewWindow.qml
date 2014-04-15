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
    property int windowRadius: 6
    property var themeObject

    property var dconstants: DConstants {}
    property int pointer: 0
    property var previewsImages: [
        "/usr/share/personalization/themes/Deepin/wallpappers/new-1.JPG",
        "/usr/share/personalization/themes/Deepin/wallpappers/new-2.JPG",
        "/usr/share/personalization/themes/Deepin/wallpappers/new-3.JPG",
        "/usr/share/personalization/themes/Deepin/wallpappers/time-1.JPG",
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
        previewsWindow.previewsImages = getPreviewPictures()
        previewSlide.view.incrementCurrentIndex()
        previewsWindow.show()
        previewsWindow.showPreviewWindow = true
        rootWindow.clickedToHide = false
    }

    function hideWindow(){
        previewsWindow.hide()
        previewsWindow.showPreviewWindow = false
        hideToTrue.restart()
    }

    Timer{
        id: hideToTrue
        interval: 200
        running: false
        onTriggered: {
            rootWindow.clickedToHide = true
        }
    }

    Item {
        anchors.fill: parent
        clip: true

        Slide {
            id: previewSlide
            anchors.right: parent.right
            width: itemWidth * previewsImages.length
            height: itemHeight
            itemWidth: 300
            itemHeight: 500
            model: previewsImages
            delegate: PreviewImage {
                radius: windowRadius
                imageSource: previewsImages[index]
                width: previewSlide.itemWidth
                height: previewSlide.itemHeight
                onClicked: {
                    if(side == "left"){
                        previewSlide.view.decrementCurrentIndex()
                    }
                    else{
                        previewSlide.view.incrementCurrentIndex()
                    }
                }
            }

            Component.onCompleted: {
                view.incrementCurrentIndex()
            }
        }

        Item{
            width: parent.width
            height: 20
            anchors.bottom: parent.bottom
            anchors.bottomMargin: bottomToolBar.height + 10

            Row {
                anchors.centerIn: parent
                height: parent.height
                spacing: 20

                Repeater {
                    model: previewsImages.length

                    Rectangle {
                        width: 10; height: 10
                        radius: 5
                        border.width: 1
                        border.color: Qt.rgba(0, 0, 0, 0.5)
                        property int realIndex: previewSlide.currentIndex == 0 ? previewsImages.length - 1 : previewSlide.currentIndex - 1
                        color: realIndex == index ? dconstants.activeColor : dconstants.fgColor

                        MouseArea {
                            width: 20; height: 20
                            anchors.centerIn: parent
                            hoverEnabled: true
                            onEntered: {
                                cursorShape = Qt.PointingHandCursor
                            }

                            onExited: {
                                cursorShape = Qt.ArrowCursor
                            }

                            onReleased: {
                                if(index == previewsImages.length -1 ){
                                    previewSlide.currentIndex = 0
                                }
                                else{
                                    previewSlide.currentIndex = index + 1
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Rectangle{
        anchors.fill: parent
        radius: windowRadius
        border.color: Qt.rgba(1, 1, 1, 0.3)
        border.width: 1
        color: "transparent"

        Rectangle{
            id: bottomToolBar
            anchors.bottom: parent.bottom
            width: parent.width
            height: 40
            color: Qt.rgba(0, 0, 0, 0.5)

            NewButton {
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 10
                text: "Apply"

                onClicked: {
                    previewsWindow.hideWindow()
                }
            }
        }
    }
}
