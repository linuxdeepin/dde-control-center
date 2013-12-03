import QtQuick 2.0

Item {
    id:root
    property list<Item> datas
    default property alias _onlyWorkaround_: root.datas
    width:parent.width
    Component {
	id:seperator 
	DSepratorHorizontal{}
    }
    Column {
	DSepratorHorizontal{}
	Repeater {
	    model: datas.length
	    delegate: Component {
		Column {
		    width: root.width
		    id: i
		    Component.onCompleted: {
			i.children = root.datas[index]
			if (root.datas[index].width != 0) { //Let Loader(or other Component) with empty content work with this trick
			    seperator.createObject(i).width = root.width
			}
		    }
		}
	    }
	}
    }
}
