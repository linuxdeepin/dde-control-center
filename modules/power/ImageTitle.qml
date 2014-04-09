import QtQuick 2.1
import Deepin.Widgets 1.0

Item {
	id: image_title
	property int spacing: 5
	property url imageSource
	property string title

	width: parent.width
	height: Math.max(image.height, title.height)

	Image {
		id: image
		source: image_title.imageSource
		anchors.verticalCenter: parent.verticalCenter
	}

	DssH2 {
		id: title
		text: image_title.title
		anchors.verticalCenter: parent.verticalCenter
		anchors.left: image.right
		anchors.leftMargin: image_title.spacing
	}
}