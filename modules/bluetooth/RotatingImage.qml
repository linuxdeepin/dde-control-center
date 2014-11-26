import QtQuick 2.1
import Deepin.Widgets 1.0

Item {
    id: item
    width: img.width
    height: img.height

    property bool rotating: false

    onRotatingChanged: rotating_animation.running = rotating

    DImageButton {
        id: img
        normal_image: "images/reload_normal.png"
        hover_image: "images/reload_hover.png"
        press_image: "images/reload_press.png"
    }

    PropertyAnimation {
        id: rotating_animation
        target: img
        property: "rotation"
        from: -360
        to: 0
        duration: 1000
        loops: Animation.Infinite
        /* easing.type: Easing.OutQuad */
    }
}
