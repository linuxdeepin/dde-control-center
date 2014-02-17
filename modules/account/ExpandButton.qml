import QtQuick 2.1
import Deepin.Widgets 1.0

DImageButton {
    id: root
    state: "not-expanded"
    property bool expanded: false

    states: [
        State{
            name: "not-expanded"
            PropertyChanges {
                target: root
                normal_image: "images/arrow_down_normal.png"
                hover_image: "images/arrow_down_hover.png"
                press_image: "images/arrow_down_hover.png"
            }
        },
        State{
            name: "expanded"
            PropertyChanges {
                target: root
                normal_image: "images/arrow_up_normal.png"
                hover_image: "images/arrow_up_hover.png"
                press_image: "images/arrow_up_hover.png"
            }
        }        
    ]
    
    onClicked: {
        root.state = root.state == "expanded" ? "not-expanded" : "expanded"
    }
}
