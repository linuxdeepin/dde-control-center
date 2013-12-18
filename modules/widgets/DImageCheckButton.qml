import QtQuick 2.1

Rectangle {
    id: imageCheckButton
    width: currentImage.width
    height: currentImage.height
    color: "transparent"

    property url inactivatedNomralImage
    property url inactivatedHoverImage
    property url inactivatedPressImage

    property url activatedNomralImage
    property url activatedHoverImage
    property url activatedPressImage

    signal clicked
    property bool active: false

    property bool hover: false
    property bool pressed: false

    onActiveChanged: {
        changeStatus()
    }
    onHoverChanged: {
        changeStatus()
    }
    onPressedChanged: {
        changeStatus()
    }

    function changeStatus(){
        if(active){
            if(pressed){
                currentImage.source = activatedPressImage
            }
            else{
                if(hover){
                    currentImage.source = activatedHoverImage
                }
                else{
                    currentImage.source = activatedNomralImage
                }
            }
        }else{
            if(pressed){
                currentImage.source = inactivatedPressImage
            }
            else{
                if(hover){
                    currentImage.source = inactivatedHoverImage
                }
                else{
                    currentImage.source = inactivatedNomralImage
                }
            }
        }
    }

    Image {
        id: currentImage
        anchors.centerIn: parent
        Component.onCompleted: {
            if(active){
                source = activatedNomralImage
            }
            else{
                source = inactivatedNomralImage
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onPressed: {
            parent.hover = false
            parent.pressed = true
        }

        onReleased: {
            parent.pressed = false
            if (containsMouse){
                parent.hover = true
            }
            else {
                parent.hover = false
            }
            parent.active = !parent.active
            imageCheckButton.clicked()
        }

        onEntered: {
            parent.hover = true
            parent.pressed = false
        }

        onExited: {
            parent.hover = false
            parent.pressed = false
        }
    }
}
