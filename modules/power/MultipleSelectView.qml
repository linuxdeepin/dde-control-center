import QtQuick 2.1

GridView {

	interactive: false
    cellWidth: width / columns
    cellHeight: height / rows
	
	property int rows: 1
	property int columns: 3
	property int viewWidth: 80
	property int viewHeight: 24
	
	property var selectedIndexs: []
	property var selectedItems: []
	
	signal select(int index, var item)
	signal deselect(int index, var item)
	
    delegate: SelectViewDelegate{}
}