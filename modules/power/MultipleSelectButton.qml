import QtQuick 2.1

Rectangle {
	id: select_button
	color: Qt.rgba(0, 0, 0, 0)
	
	width:  gridview.width + left_img.width + right_img.width
	height: cellHeight
	
    property int cellWidth: 60
    property int cellHeight: 30
	property bool singleSelectionMode: false
	
	property alias model: gridview.model
	property alias selectedIndexs: gridview.selectedIndexs
	property alias selectedItems: gridview.selectedItems
	
    signal select(var index, var item)
    signal deselect(var index, var item)
	
    Image {
        id: left_img
        source: normalImage
		
		property string normalImage: "images/button_left_normal.png"
		property string pressedImage: "images/button_left_pressed.png"

        anchors {
            left: parent.left
			verticalCenter: parent.verticalCenter
        }
    }

    GridView {
		id: gridview
		
        width: cellWidth * model.count
        height: cellHeight
		interactive: false

        cellWidth: parent.cellWidth
        cellHeight: parent.cellHeight

        property var selectedIndexs: []
        property var selectedItems: []
		property alias singleSelectionMode: select_button.singleSelectionMode

		function select(index, item) {
			if (index == 0) {
				left_img.source = left_img.pressedImage
			}
			if (index == count - 1) {
				right_img.source = right_img.pressedImage
			}
			
			select_button.select(index, item)
		}

		function deselect(index, item) {
			if (index == 0) {
				left_img.source = left_img.normalImage
			}
			if (index == count - 1) {
				right_img.source = right_img.normalImage
			}
			
			select_button.deselect(index, item)
		}
		
		function clear() {
			for (var index in selectedItems) {
				deselect(selectedIndexs[index], selectedItems[index])
				selectedItems[index].pressed = false
				selectedItems[index].imageSource = selectedItems[index].normalImage
                selectedItems[index].textColor = selectedItems[index].fontNormalColor
			}
			selectedItems = []
			selectedIndexs = []
		}

        delegate: SelectButtonDelegate{}
		
		anchors {
			left: left_img.right
		}
    }

    Image {
        id: right_img
        source: normalImage
		
		property string normalImage: "images/button_right_normal.png"
		property string pressedImage: "images/button_right_pressed.png"

        anchors {
            left: gridview.right
			verticalCenter: parent.verticalCenter			
        }
    }
}
