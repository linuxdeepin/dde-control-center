import QtQuick 2.1
import QtGraphicalEffects 1.0
import QtQuick.Window 2.1
import Deepin.Widgets 1.0
import QtQml.Models 2.1
import "../../shared"

Window {
    id: previewsWindow

    width: rootFrame.width + (frameRadius + shadowRadius) * 2
    height: rootFrame.height + (frameRadius + shadowRadius) * 2
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool 

    color: "transparent"
    property int frameRadius: 4
    property int shadowRadius: 10

    property int contentWidth: 300
    property int contentHeight: 500

    property var themeObject
    property var dconstants: DConstants {}
    property int pointer: 0
    property var previewsImages: ["/usr/share/backgrounds/default_background.jpg"]
    property bool isVisible: false

    onThemeObjectChanged: {
        if(themeObject)
            previewsImages = getPreviewPictures()
    }

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
        previewSlide.currentIndex = 1
        previewsWindow.show()
        previewsWindow.isVisible = true
        rootWindow.clickedToHide = false
    }

    function hideWindow(){
        previewsWindow.hide()
        previewsWindow.isVisible = false
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

    RectangularGlow {
        id: effect
        anchors.fill: rootFrame
        glowRadius: shadowRadius
        spread: 0.2
        color: Qt.rgba(0, 0, 0, 0.4)
        cornerRadius: rootFrame.radius + glowRadius
    }

    Rectangle {
        id: rootFrame
        anchors.centerIn: parent
        width: contentWidth 
        height: contentHeight
        radius: frameRadius
        color: Qt.rgba(0, 0, 0, 0.4)

        Item {
            width: contentWidth
            height: contentHeight
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
                    radius: frameRadius
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
                                width: 25; height: 25
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
            width: contentWidth
            height: contentHeight

            radius: frameRadius
            border.color: Qt.rgba(1, 1, 1, 0.3)
            border.width: 1
            color: "transparent"

            MouseArea{
                width: parent.width
                height: 100
                hoverEnabled: true

                onEntered: topToolBar.show()
                onExited: topToolBar.hide()
            }

            Rectangle{
                id: topToolBar
                width: parent.width
                height: 40
                color: "transparent"
                visible: opacity > 0
                opacity: 0

                RadiusRect{
                    width: parent.width
                    height: parent.height
                    stroke: false
                    fillStyle: Qt.rgba(0, 0, 0, 0.5)
                    topLeftRadius: frameRadius
                    topRightRadius: frameRadius
                    bottomLeftRadius: 0
                    bottomRightRadius: 0
                }

                function show(){
                    opacity = 1
                }

                function hide(){
                    hideTimer.restart()
                }

                Timer{
                    id: hideTimer
                    interval: 200
                    onTriggered: {
                        if(!closeButton.hovered && !closeButton.pressed){
                            topToolBar.opacity = 0
                        }
                    }
                }

                Behavior on opacity { 
                    NumberAnimation { duration: 200 }
                }

                CustomButton {
                    id: closeButton
                    anchors.right: parent.right
                    anchors.rightMargin: 12
                    anchors.verticalCenter: parent.verticalCenter
                    xPadding: 2

                    contentLoader.sourceComponent: Image{ source: "images/close_normal.png" }

                    onClicked: {
                        previewsWindow.hideWindow()
                    }
                }
            }

            RadiusRect {
                id: bottomToolBar
                anchors.bottom: parent.bottom
                width: parent.width
                height: 40
                stroke: false
                fillStyle: Qt.rgba(0, 0, 0, 0.5)
                topLeftRadius: 0
                topRightRadius: 0
                bottomLeftRadius: frameRadius
                bottomRightRadius: frameRadius

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                }

                CustomButton {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 10

                    contentLoader.sourceComponent: DLabel {
                        text: "Apply"
                        anchors.centerIn: parent

                        color: {
                            if(parent.pressed){
                                return dconstants.activeColor
                            }
                            else if (parent.hovered){
                                return dconstants.hoverColor
                            }
                            else{
                                return dconstants.fgColor
                            }
                        }
                    }

                    onClicked: {
                        previewsWindow.hideWindow()
                    }
                }
            }
        }
    }
}
