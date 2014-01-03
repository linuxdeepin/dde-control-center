import QtQuick 2.0
import QtQml.Models 2.1
import Deepin.Widgets 1.0

DBaseExpand {
    id: expand

    property var keyBindings
    expanded: expandItemIndex == myIndex
    
    onExpandedChanged: {
        header.item.active = expanded
    }
    
    header.sourceComponent: DDownArrowHeader {
        text: name
        width: parent.width
        leftMargin: 25
        rightMargin: 15
        
        Component.onCompleted: {
            active = expand.expanded
        }
        
        onClicked: {
            expandItemIndex = active ? myIndex : -1
        }
    }
    
    content.sourceComponent: Component {
        ListView {
            id: lists
            focus: true
            width: parent.width
            height: lists.count * 30 > 300 ? 200 : lists.count * 30
            model: keyBindings.length
            property var keyData: keyBindings
            delegate: ShortcutInput {
                info: keyBindings[index]
            }
        }
    }
}
