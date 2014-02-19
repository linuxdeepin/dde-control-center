import QtQuick 2.1
import Deepin.Widgets 1.0

DImageButton {
    id: root
    property string status: "normal"
    property bool up: false

    normal_image: root.up ? "images/arrow_up_" + status + ".png" : "images/arrow_down_" + status + ".png"
    hover_image: root.up ? "images/arrow_up_" + status + ".png" : "images/arrow_down_" + status + ".png"
    press_image: root.up ? "images/arrow_up_" + status + ".png" : "images/arrow_down_" + status + ".png"

    onClicked: {
        root.up = !root.up
    }
}
