import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    rightLoader.sourceComponent: DTextInput{
        width: valueWidth
        Binding on text {
            when: root.value != undefined
            value: root.value
        }
        onTextChanged: {
            root.value = text
            setKey()
        }
    }
}

