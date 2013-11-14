import QtQuick 2.1

Rectangle {
	id: module
    width: 300
	height: 600
	
	TitleSeparator {
		width: module.width 
		height: module.height
		
		titleIcon: "images/icon.png"
		titleText: qsTr("Desktop Icons")
	}

    Column {
        CheckBox {
            text: qsTr("Breakfast")
        }
        CheckBox {
            text: qsTr("Lunch")
        }
        CheckBox {
            text: qsTr("Dinner")
        }
    }
}
