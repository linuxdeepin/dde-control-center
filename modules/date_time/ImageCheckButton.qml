import QtQuick 2.1

Rectangle {
    id: imageCheckButton
    width: currentImage.width
    height: currentImage.height
    color: "transparent"

    property string inactivatedNomralImage
    property string inactivatedHoverImage
    property string inactivatedPressImage

    property string activatedNomralImage
    property string activatedHoverImage
    property string activatedPressImage

    property bool activate: false

    property bool hover: false

    Image {
        id: currentImage
        anchors.centerIn: parent
        source: inactivatedNomralImage
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onPressed: {
            if (imageCheckButton.activate) {
                currentImage.source = activatedPressImage
            }
            else {
                currentImage.source = inactivatedPressImage
            }
        }

        onReleased: {
            imageCheckButton.activate = !imageCheckButton.activate
            if (imageCheckButton.activate) {
                currentImage.source = activatedHoverImage
            }
            else {
                currentImage.source = inactivatedHoverImage
            }
        }

        onEntered: {
            imageCheckButton.hover = true
            if (imageCheckButton.activate) {
                currentImage.source = activatedHoverImage
            }
            else {
                currentImage.source = inactivatedHoverImage
            }
        }

        onExited: {
            imageCheckButton.hover = false
            if (imageCheckButton.activate) {
                currentImage.source = activatedNomralImage
            }
            else {
                currentImage.source = inactivatedNomralImage
            }
        }
    }
}
