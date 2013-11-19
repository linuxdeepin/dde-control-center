import QtQuick 2.1

Rectangle {
    id: imageCheckButton

    property string inactivatedNomralImage
    property string inactivatedHoverImage
    property string inactivatedPressImage

    property string activatedNomralImage
    property string activatedHoverImage
    property string activatedPressImage

    property bool activated: false
    property bool selected: false

    property bool hover: false

    signal clicked

    onSelectedChanged: {
        if (selected && hover){
            currentImage.source = activatedHoverImage
            activated = true
        }
        else if (selected && !hover) {
            currentImage.source = activatedNomralImage
            activated = true
        }
        else if (!selected && hover) {
            currentImage.source = inactivatedHoverImage
            activated = false
        }
        else if (!selected && !hover) {
            currentImage.source = inactivatedNomralImage
            activated = false
        }
    }

    Image {
        id: currentImage
        anchors.centerIn: parent
        source: inactivatedNomralImage
        scale: 0.875
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onClicked: {
            imageCheckButton.clicked() 
            if (!imageCheckButton.selected) {
                imageCheckButton.activated = !imageCheckButton.activated
            }
        }

        onPressed: {
            if (imageCheckButton.activated) {
                currentImage.source = activatedPressImage
            }
            else {
                currentImage.source = inactivatedPressImage
            }
        }

        onReleased: {

            if (imageCheckButton.activated) {
                currentImage.source = activatedHoverImage
            }
            else {
                currentImage.source = inactivatedHoverImage
            }
        }

        onEntered: {
            imageCheckButton.hover = true
            if (imageCheckButton.activated) {
                currentImage.source = activatedHoverImage
            }
            else {
                currentImage.source = inactivatedHoverImage
            }
        }

        onExited: {
            imageCheckButton.hover = false
            if (imageCheckButton.activated) {
                currentImage.source = activatedNomralImage
            }
            else {
                currentImage.source = inactivatedNomralImage
            }
        }
    }
}
