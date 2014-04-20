import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    rightLoader.sourceComponent: DTextInput{
        id: passwordArea
        width: valueWidth
        onTextChanged: {
            root.value = text
            setKey()
        }
        Component.onCompleted: {
            if (root.visible) {
                text = getKey()
            }
        }
    }
}

