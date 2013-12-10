import QtQuick 2.1
import QtGraphicalEffects 1.0

Item {
    id: round_item
    width: 100
    height: 100

    property int radius: 50
    default property alias source: target.sourceComponent
	
	Loader {
		id: target
	}

    Rectangle {
        id: mask
        radius: round_item.radius
        smooth: true
        antialiasing: true

        anchors.fill: parent
    }

    OpacityMask {
        anchors.fill: parent
		
        source: target
        maskSource: mask
		
		Component.onCompleted: {
			target.visible = false
		}
    }
}