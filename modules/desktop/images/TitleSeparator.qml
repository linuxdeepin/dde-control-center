import QtQuick 2.1

Rectangle {
	id: sep
    property string titleIcon: ""
    property string titleLabel: ""

    Row {
        Column {
            Image {
                source: titleIcon
            }
            Text {
                text: titleLabel
            }
        }
        Rectangle {
            width: 1
            height: sep.width
            rotation: 90
            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.rgba(0, 0, 0, 1) }
                GradientStop { position: 1.0; color: Qt.rgba(1, 1, 1, 1) }
            }
        }
    }
}