import QtQuick 2.1

Column {
    id: slider
    width: 300

    property bool fill: true
    property bool showValue: true
    property real percentage: 0.0
    property string value: "0"
    property url imageSource: "images/slider_handle.svg"

    Item {
        width: parent.width
        height: 20

        Text {
            x: handle.x + (handle.width - implicitWidth) / 2
            text: slider.value
            
            
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    SliderBg {
        id: bg
        width: parent.width
        height: 10

        Rectangle {
            width: handle.x + handle.width / 2
            height: parent.height
            radius: parent.radius
            anchors.verticalCenter: parent.verticalCenter
            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.rgba(0, 104/255, 170/255, 1.0) }
                GradientStop { position: 1.0; color: Qt.rgba(91/255, 164/255, 211/255, 1.0) }
            }
        }

        Image {
            id: handle
            source: slider.imageSource
            anchors.verticalCenter: parent.verticalCenter

            x: slider.percentage * bg.width - handle.width / 2

            onXChanged: {
                slider.percentage = (handle.x + handle.width / 2)  / bg.width
            }

            MouseArea {
                anchors.fill: parent

                drag.target: handle
                drag.axis: Drag.XAxis
                drag.minimumX: -handle.width / 2
                drag.maximumX: bg.width - handle.width / 2
            }
        }
    }
}