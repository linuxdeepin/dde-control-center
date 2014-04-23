import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 1.1
import QtGraphicalEffects 1.0

Window {
    width: 300
    height: 300
    color: "transparent"
    flags: Qt.Tool | Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint

    RectangularGlow {
        id: effect
        anchors.fill: rect
        glowRadius: 10
        spread: 0.2
        color: Qt.rgba(0, 0, 0, 0.6)
        cornerRadius: rect.radius + glowRadius
    }

    Rectangle {
        id: rect
        color: Qt.rgba(0, 0, 0, 0.6)
        anchors.centerIn: parent
        width: Math.round(parent.width / 1.5)
        height: Math.round(parent.height / 2)
        radius: 4

        Column {
            width: parent.width
            Slider{
                id: rectSlider
                width: parent.width - 10
                minimumValue: 0
                maximumValue: 1.0
                onValueChanged: {
                    rect.color = Qt.rgba(0, 0, 0, value)
                }
            }

            Slider{
                id: effectSlider
                width: parent.width - 10
                minimumValue: 0
                maximumValue: 1.0
                onValueChanged: {
                    effect.color = Qt.rgba(0, 0, 0, value)
                }
            }

            Button {
                text: "Get"
                onClicked: print("Rect: " + rectSlider.value, "effect: " + effectSlider.value)
            }
        }
    }
}
