import QtQuick 2.0
import QtQml.Models 2.1
import Deepin.Widgets 1.0

Column {
    width: parent.width

    property bool isConflictValid: false
    property bool isConflictInvalid: false

    DBaseExpand {
        id: expand

        expanded: expandItemIndex == myIndex
        
        onExpandedChanged: {
            header.item.active = expanded
        }
        
        header.sourceComponent: DDownArrowHeader {
            text: name
            width: parent.width
            icon: {
                if(isConflictInvalid){
                    return "images/error_key.png"
                }
                else if(isConflictValid){
                    return "images/conflict_key.png"
                }
                else{
                    return ""
                }
            }
            leftMargin: icon ? 20 : 0
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
                    warning: {
                        for(var i in conflictValid){
                            if (keyBindings[index][0] == conflictValid[i]){
                                isConflictValid = true
                                return "conflict"
                            }
                        }
                        for(var i in conflictInvalid){
                            if (keyBindings[index][0] == conflictInvalid[i]){
                                isConflictInvalid = true
                                return "error"
                            }
                        }
                        return ""
                    }
                }
            }
        }
    }

    DSeparatorHorizontal {}
}
