import QtQuick 2.1

Rectangle {
    id: imageButton

    property string nomralImage
    property string hoverImage
    property string pressImage

    property bool hover: false
    property alias image: currentImage

    width: currentImage.width
    height: currentImage.height
    color: "transparent"

    signal clicked

    Image {
        id: currentImage
        anchors.centerIn: parent
        source: nomralImage
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onClicked: { imageButton.clicked() }

        onPressed: {
            currentImage.source = pressImage
        }

        onReleased: {
            currentImage.source = hoverImage
        }

        onEntered: {
            imageButton.hover = true
            currentImage.source = hoverImage
        }

        onExited: {
            imageButton.hover = false
            currentImage.source = nomralImage
        }
    }
}
