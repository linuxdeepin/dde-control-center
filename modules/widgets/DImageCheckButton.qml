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
    property bool activate: false

    property bool hover: false
    property bool pressed: false

    function changeStatus(){
        if(activate){
            if(pressed){
                currentImage.source = activatedPressImage
            }
        }else{
        }
    }

    Image {
        id: currentImage
        anchors.centerIn: parent
        source: inactivatedNomralImage
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
            parent.activate = !parent.activate
            imageCheckButton.clicked()
        }

        onEntered: {
            parent.hover = true
            parent.pressed = false
            print(activate, hover, pressed)
            print(currentImage.source)
        }

        onExited: {
            parent.hover = false
            parent.pressed = false
            print(activate, hover, pressed)
            print(currentImage.source)
        }
    }
}
