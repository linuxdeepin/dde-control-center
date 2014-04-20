import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    rightLoader.sourceComponent: DTextInput{
        id: passwordArea
        width: valueWidth
        text: root.value
        onActiveFocusChanged: {
            if(!activeFocus){
                root.value = text
                setKey()
            }
        }
    }
    
}

