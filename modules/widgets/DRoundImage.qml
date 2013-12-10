import QtQuick 2.1

Rectangle {
	id: round_image
	width: (roundRadius + borderWidth) * 2
	height: (roundRadius + borderWidth) * 2
	radius: roundRadius + borderWidth
	color: borderColor
	
	property int roundRadius: 50
	property int borderWidth: 3
	property color borderColor: "white"
	property string imageSource: ""
	
    DRoundItem {
		radius: parent.roundRadius
		
        Image {
            source: round_image.imageSource
        }
		
		anchors.centerIn: parent
    }
}
