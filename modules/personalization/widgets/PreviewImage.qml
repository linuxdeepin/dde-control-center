import QtQuick 2.1
import QtGraphicalEffects 1.0
import Deepin.Widgets 1.0

Item {
    id: previewImage

    property int radius: 6
    property url imageSource: ""

    signal clicked(int index, string side)

    Image{
        id: bg
        source: imageSource
        width: parent.width
        height: parent.height
        smooth: true
        visible: false
    }

    Rectangle{
        id: borderRect
        width: parent.width
        height: parent.height
        radius: previewImage.radius
        smooth: true
        visible: false
    }

    OpacityMask{
        anchors.fill: bg
        source: bg
        maskSource: borderRect
    }

    MouseArea{
        width: parent.width/2
        height: parent.height
        hoverEnabled: true
        property url leftCursor: "images/left_normal.png"
        onClicked: {
            parent.clicked(index, "left")
        }

        onEntered: windowView.setCustomCursor(leftCursor)
        onExited: windowView.clearCustomCursor()
    }

    MouseArea{
        width: parent.width/2
        height: parent.height
        anchors.right: parent.right
        hoverEnabled: true
        property url rightCursor: "images/right_normal.png"
        onClicked: {
            parent.clicked(index, "right")
        }
        onEntered: windowView.setCustomCursor(rightCursor)
        onExited: windowView.clearCustomCursor()
    }
}
