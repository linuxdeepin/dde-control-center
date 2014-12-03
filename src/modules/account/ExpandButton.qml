import QtQuick 2.1
import Deepin.Widgets 1.0

DImageButton {
    id: root
    property bool up: false
    property string status: "normal"

    NumberAnimation {
        id: rotation_animation
        target: root
        property: "rotation"
        to: up ? root.rotation - 180 : root.rotation + 180
        duration: 200
    }

    onUpChanged: {
        rotation_animation.start()
    }

    /* normal_image: root.up ? "images/arrow_up_" + status + ".png" : "images/arrow_down_" + status + ".png" */
    /* hover_image: root.up ? "images/arrow_up_" + status + ".png" : "images/arrow_down_" + status + ".png" */
    /* press_image: root.up ? "images/arrow_up_" + status + ".png" : "images/arrow_down_" + status + ".png" */

    normal_image: "images/arrow_down_" + status + ".png"
    hover_image: "images/arrow_down_" + status + ".png"
    press_image: "images/arrow_down_" + status + ".png"
}
