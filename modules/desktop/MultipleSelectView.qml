import QtQuick 2.1

Rectangle {
    color: "transparent"
	
    property bool radioMode: false	
	
	property int rows: 1
	property int columns: 3
	
	property int topPadding: 10
	property int bottomPadding: 10
	property int leftPadding: 15
	property int rightPadding: 15
	property int horizontalSpacing: 10
	property int verticalSpacing: 10

    GridView {
        interactive: false

        property var selectedIndexs: []
        property var selectedItems: []

        signal itemSelected(int index, var item)
        signal itemDeselected(int index, var item)

        function clearSelection() {
            for (var index in selectedItems) {
                selectedItems[index].deselect()
            }
			
            selectedItems = []
            selectedIndexs = []
        }

        function getDelegateInstanceAt(index) {
            for(var i = 0; i < contentItem.children.length; ++i) {
                var item = contentItem.children[i];
                if ((typeof item.delegateIndex != "undefined") && item.delegateIndex == index) {
                    return item
                }
            }
            return undefined
        }

        delegate: SelectViewDelegate{}
    }
}
