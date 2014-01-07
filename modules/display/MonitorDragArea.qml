import QtQuick 2.0
import Deepin.Widgets 1.0

Rectangle {
    width: parent.width 
    height: 180
    color: dconstants.contentBgColor
    clip: true

    Rectangle{
        id: r1
        width: 120
        height: 90
        color: "#0d0d0d"
        x: (parent.width-width)/2
        y: (parent.height-height)/2
        border.width: 1
        border.color: dconstants.fgDarkColor

        DssH2 {
            anchors.centerIn: parent
            text: "Main"
        }

        MouseArea {
            anchors.fill: parent
            drag.target: parent
            drag.axis: Drag.XAndYAxis
            onPressed: {
                parent.color = "#252525"
                parent.opacity = 0.5
            }
            onReleased: {
                parent.color = "#0d0d0d"
                releaseCal(r1, r2)
            }
        }

    }

    Rectangle{
        id: r2
        width: 100
        height: 98
        color: "#0d0d0d"
        x: (parent.width-width)/2
        y: (parent.height-height)/2
        border.width: 1
        border.color: dconstants.fgDarkColor
        Behavior on x { SpringAnimation { spring: 3; damping: 0.3; mass: 1.0 } }
        Behavior on y { SpringAnimation { spring: 3; damping: 0.3; mass: 1.0 } }

        DssH2 {
            anchors.centerIn: parent
            text: "Attach"
        }

        MouseArea {
            anchors.fill: parent
            drag.target: parent
            drag.axis: Drag.XAndYAxis
            onPressed: {
                parent.color = "#252525"
                parent.opacity = 0.5
            }
            onReleased: {
                parent.color = "#0d0d0d"
                releaseCal(r1, r2)
            }
        }
    }

    function releaseCal(main_obj, attach_obj) {
        attach_obj.x = main_obj.x + main_obj.width
        attach_obj.y = main_obj.y
    }

    Component.onCompleted: {
        var main_obj = r1
        var attach_obj = r2
        main_obj.x = (width - main_obj.width - attach_obj.width)/2
        main_obj.y = main_obj.height > attach_obj.height ? (height - main_obj.height)/2 : (height - attach_obj.height)/2
        attach_obj.x = main_obj.x + main_obj.width
        attach_obj.y = main_obj.y
    }
}
