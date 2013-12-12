import QtQuick 2.1

Rectangle {
	id: round_image
	
	property int roundRadius: 50
	property int borderWidth: 3
	property color borderColor: "white"
	property url imageSource: ""
    
	width: (roundRadius + borderWidth) * 2
	height: (roundRadius + borderWidth) * 2
	radius: roundRadius + borderWidth
	color: borderColor
	
    DRoundItem {
        width: radius * 2
        height: radius * 2
		radius: parent.roundRadius
		
        Image {
            source: round_image.imageSource
        }
		
		anchors.centerIn: parent
    }
}
