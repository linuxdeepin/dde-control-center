import QtQuick 2.1
import Deepin.Widgets 1.0

Item {
    id: root
    width: zoom_out_button.width + zoom_in_button.width + 8 * 2 + zoom_bg.width
    height: zoom_out_button.height

    property real setp: 0.1
    property real percentage: 0
    property real mostDistance: zoom_bg.implicitWidth - zoom_slider.implicitWidth

    DImageButton {
        id: zoom_out_button

        normal_image: "images/zoomout.png"
        hover_image: "images/zoomout.png"
        press_image: "images/zoomout_press.png"

        onClicked: root.percentage = Math.max(0, root.percentage - 0.1)
    }

    Image {
        id: zoom_bg
        source: "images/zoom_background.png"

        MouseArea {
            anchors.fill: parent

            onClicked: zoom_slider.x = mouse.x - zoom_slider.width / 2
        }

        Image {
            id: zoom_slider
            source: "images/zoom_slider.png"

            x: root.percentage * root.mostDistance

            onXChanged: {
                root.percentage = zoom_slider.x / root.mostDistance
            }

            MouseArea {
                anchors.fill: parent

                drag.target: zoom_slider
                drag.axis: Drag.XAxis
                drag.minimumX: 0
                drag.maximumX: root.mostDistance
            }
        }

        anchors.left: zoom_out_button.right
        anchors.leftMargin: 8
        anchors.verticalCenter: parent.verticalCenter
    }

    DImageButton {
        id: zoom_in_button

        normal_image: "images/zoomin.png"
        hover_image: "images/zoomin.png"
        press_image: "images/zoomin_press.png"

        onClicked: root.percentage = Math.min(1, (zoom_slider.x / root.mostDistance) + 0.1)

        anchors.left: zoom_bg.right
        anchors.leftMargin: 8
    }
}