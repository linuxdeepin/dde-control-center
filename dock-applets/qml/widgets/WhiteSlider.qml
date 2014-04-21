import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

Slider {
    id: slider
    width: 196
    property int grooveWidth: width - handleWidth
    property int grooveHeight: 8
    property int handleWidth: 12

    property int _buttonWidth: 0

    style: SliderStyle {
        groove: Rectangle {
            implicitWidth: grooveWidth + handleWidth
            implicitHeight: 2
            color: Qt.rgba(1, 1, 1, 0.1)

            Rectangle {
                width: slider.value/(slider.maximumValue - slider.minimumValue) * parent.width
                height: parent.height
            }

        }
        handle: Rectangle {
            width: handleWidth
            height: handleWidth
            radius: handleWidth/2
        }
    }

}

