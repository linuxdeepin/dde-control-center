import QtQuick 2.1
import Deepin.Widgets 1.0

DImageButton {
    id: root
    state: "normal"
    property bool up: false
    
    states: [
        State {
            name: "normal"
            PropertyChanges {
                target: root
                normal_image: root.up ? "images/arrow_up_normal.png" : "images/arrow_down_normal.png"
                hover_image: root.up ? "images/arrow_up_normal.png" : "images/arrow_down_normal.png"
                press_image: root.up ? "images/arrow_up_normal.png" : "images/arrow_down_normal.png"
            }
        },
        State {
            name: "hover"
            PropertyChanges {
                target: root
                normal_image: root.up ? "images/arrow_up_hover.png" : "images/arrow_down_hover.png"
                hover_image: root.up ? "images/arrow_up_hover.png" : "images/arrow_down_hover.png"
                press_image: root.up ? "images/arrow_up_hover.png" : "images/arrow_down_hover.png"
            }
        }
    ]

    onClicked: {
        root.up = !root.up
    }
}
