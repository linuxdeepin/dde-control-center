import QtQuick 2.0
import Deepin.Widgets 1.0

Column {
    width: parent.width
    height: childrenRect.height

    property string categoryName: name
    property int myIndex: index
    property var categoryKeyBindings: bindManagerId[propertyName]

    DBaseExpand {
        id: expand
        expanded: expandItemIndex == myIndex

        onExpandedChanged: {
            header.item.active = expanded
        }

        header.sourceComponent: DDownArrowHeader {
            text: categoryName
            width: parent.width
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
                height: childrenRect.height
                model: categoryKeyBindings.length
                delegate: ShortcutInput {
                    info: categoryKeyBindings[index]
                }
            }
        }
    }

    DSeparatorHorizontal {}
}
