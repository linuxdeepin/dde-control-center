import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    rightLoader.sourceComponent: Row {
    DTextInput {
        width: valueWidth
        visible: connectionSessionObject.connectionType != "wired"
        Binding on text {
            when: root.value != undefined
            value: root.value
        }
        // TODO
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
    DLabel {
        width: valueWidth
        visible: {
            print("->", connectionSessionObject.connectionType)
            return connectionSessionObject.connectionType == "wired"
        }
        Binding on text {
            when: root.value != undefined
            value: root.value
        }
        // TODO
        // text: {
        //     if (root.value) {
        //         return root.value
        //     } else {
        //         return ""
        //     }
        // }
    }
    }
}

