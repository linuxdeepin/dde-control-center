import QtQuick 2.0
import "../widgets"

Item {
    id: expandArea
    width: parent.width
    height: content.height

    property var modelComponent
    property var expandItems

    property int expandItemIndex: -1

    Column {
        id: content
        anchors.top: parent.top
        width: parent.width
        
        Repeater {
            id: repeater
            model: expandArea.expandItems.length
            property alias items: expandArea.expandItems
            delegate: DBaseExpand {
                id: expand
                expanded: expandArea.expandItemIndex == index
                
                property string currentDisplayName: ""
                
                onExpandedChanged: {
                    header.item.active = expanded
                }
                
                header.sourceComponent: DDownArrowHeader {
                    text: expandArea.expandItems[index].name
                    //icon: expandArea.expandItems[index].icon
                    width: parent.width
                    leftMargin: 25
                    rightMargin: 15
                    
                    Component.onCompleted: {
                        active = expand.expanded
                    }
                    
                    onClicked: {
                        expandArea.expandItemIndex = active ? index : -1
                    }
                }
                
                content.sourceComponent: modelComponent
                contentData: expandArea.expandItems[index]
            }
        }
    }
}
