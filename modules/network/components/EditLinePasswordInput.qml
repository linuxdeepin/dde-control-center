import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    property var echoMode
    
    rightLoader.sourceComponent: DTextInput{
        id: textInput
        width: valueWidth
        echoMode: TextInput.Password
        Binding on echoMode {
            target: root
            value: root.echoMode
        }
        // TODO fix property loop binding
        Binding on text {
            when: root.value != undefined
            value: root.value
        }
        // text: {
        //     if (root.value) {
        //         return root.value
        //     } else {
        //         return ""
        //     }
        // }
        onTextChanged: {
            root.value = text
            setKey()
        }
    }
    
}

