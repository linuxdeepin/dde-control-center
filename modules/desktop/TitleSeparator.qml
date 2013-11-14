import QtQuick 2.1

Rectangle {
    id: sep
	
	implicitHeight: sepTopMargin + sepBottomMargin + iconLabel.implicitHeight

	property color fontColor: "white"
	property int sepTopMargin: 15
	property int sepBottomMargin: 15
    property int iconLabelSpacing: 3
	property color gradientStartColor: "white"
	property color gradientStopColor: "#252627"
	
    property string titleIcon: ""
    property string titleLabel: ""
	
    Rectangle {
        anchors.fill: parent
        anchors.topMargin: sepTopMargin
        anchors.bottomMargin: sepBottomMargin

        Row {
            id: iconLabel
            spacing: sep.iconLabelSpacing
            Image {
                source: titleIcon
            }
            Text {
                text: titleLabel
				color: fontColor
            }
        }

        Rectangle {
            width: 300
            height: 1
			anchors.top: iconLabel.bottom
			anchors.topMargin: 5
			
            Rectangle {
                width: 1
                height: parent.width
                rotation: 270
				anchors.centerIn: parent
                gradient: Gradient {
                    GradientStop { position: 0.0; color: gradientStartColor}
                    GradientStop { position: 0.8; color: gradientStopColor}
                    GradientStop { position: 1.0; color: gradientStopColor}
                }
            }
        }
    }
}