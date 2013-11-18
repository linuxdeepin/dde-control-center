import QtQuick 2.1

Image {
    id: delegate

    source: normalImage

    width: delegate.GridView.view.cellWidth
    height: delegate.GridView.view.cellHeight
    fillMode: Image.Tile

    property bool pressed: false
    property color fontNormalColor: "#898989"
    property color fontPressedColor: "#19A9F9"

    property string normalImage: "images/button_center_normal.png"
    property string pressedImage: "images/button_center_pressed.png"
	
	property alias imageSource: delegate.source
	property alias textColor: txt.color

    Text {
        id: txt

        color: fontNormalColor
        text: label
        font.pixelSize: 12

        anchors.centerIn: parent
    }
	
    Rectangle {
        color: "black"
        opacity: 0.5
        visible: index != 0

        width: 1
        height: 24

        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
    }

    MouseArea {
        anchors.fill: parent

        onPressed: {
            if (!delegate.pressed) {
				
				if (delegate.GridView.view.singleSelectionMode) {
					delegate.GridView.view.clear()
				}
				
                delegate.source = delegate.pressedImage
                txt.color = delegate.fontPressedColor

                delegate.GridView.view.selectedIndexs.push(index)
                delegate.GridView.view.selectedItems.push(delegate)
                delegate.GridView.view.select(index, delegate)
            } else {
                delegate.source = delegate.normalImage
                txt.color = delegate.fontNormalColor

                var target_index_index = delegate.GridView.view.selectedIndexs.indexOf(index)
                var target_item_index = delegate.GridView.view.selectedItems.indexOf(delegate)

                delegate.GridView.view.selectedIndexs.splice(target_index_index, 1)
                delegate.GridView.view.selectedItems.splice(target_item_index, 1)
                delegate.GridView.view.deselect(index, delegate)
            }
            delegate.pressed = !delegate.pressed
        }
    }
}