import QtQuick 2.1

Item {
	id: button
	
	property string normal_image
	property string hover_image
	property string press_image
	
	signal clicked
	
	width: image.width;	height: image.height
	
	Image {
		id: image
		source: normal_image
	}
	
	MouseArea {
		id: mouseArea
		anchors.fill: parent
		hoverEnabled: true
		onEntered: { image.source = hover_image }
		onExited: { image.source = normal_image }
		onPressed: { image.source = press_image }
		onReleased: { image.source= mouseArea.containsMouse ? hover_image : normal_image}
		onClicked: button.clicked()
	}
}
